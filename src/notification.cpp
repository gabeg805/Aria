/**
 * -----------------------------------------------------------------------------
 * @file notification.hpp
 * @author Gabriel Gonzalez
 * 
 * @brief The interface for the Aria notification bubble.
 * -----------------------------------------------------------------------------
 */

#include "notification.hpp"
#include "sharedmem.hpp"
#include "commandline.hpp"
#include "config.hpp"
#include "util.hpp"
#include <gtkmm.h>
#include <gdkmm.h>
#include <time.h>
#include <unistd.h>
#include <pangomm/fontdescription.h>
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>

ARIA_NAMESPACE

/**
 * @brief Contruct the notification bubble window, widget containers, and set up
 *        various signals.
 */
notification::notification() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    bubble_(Gtk::ORIENTATION_HORIZONTAL),
    icon_(Gtk::ORIENTATION_VERTICAL),
    text_(Gtk::ORIENTATION_VERTICAL),
    background_(),
    width_(0),
    height_(0),
    xpos_(0),
    ypos_(0),
    curve_(0)
{
    this->set_decorated(false);
    this->set_app_paintable(true);
    this->signal_draw().connect(sigc::mem_fun(*this, &notification::on_draw));
    this->signal_screen_changed().connect(sigc::mem_fun(*this, &notification::on_screen_changed));
    this->on_screen_changed(get_screen());
    std::signal(SIGINT,  cleanup);
    std::signal(SIGQUIT, cleanup);
    std::signal(SIGTERM, cleanup);
}

/**
 * @brief Build the notification bubble and set all attributes.
 * 
 * @details Retrieve the values corresponding to each possible command line
 *          option. It's not expected that all of these will be set, so if they
 *          are not, the value in the config file is used.
 * 
 * @param[in] cli The command line interface, containing all the command line
 *                information.
 * 
 * @return 0 on success. Any other value is an error.
 */
int notification::build(commandline::interface& cli)
{
    std::string title        = cli.get("title");
    std::string body         = cli.get("body");
    std::string font         = cli.get("font");
    std::string titlesize    = cli.get("title-size");
    std::string bodysize     = cli.get("body-size");
    std::string icon         = cli.get("icon");
    std::string spacing      = cli.get("icon-text-spacing");
    std::string time         = cli.get("time");
    std::string xpos         = cli.get("xpos");
    std::string ypos         = cli.get("ypos");
    std::string width        = cli.get("width");
    std::string height       = cli.get("height");
    std::string background   = cli.get("background");
    std::string foreground   = cli.get("foreground");
    std::string opacity      = cli.get("opacity");
    std::string curve        = cli.get("curve");
    std::string margin       = cli.get("margin");
    std::string margintop    = cli.get("margin-top");
    std::string marginbottom = cli.get("margin-bottom");
    std::string marginleft   = cli.get("margin-left");
    std::string marginright  = cli.get("margin-right");

    if (this->set_notify_title_and_body(title, body, font, titlesize,
                                        bodysize) < 0)
    {
        return 1;
    }
    if (this->set_notify_icon(icon, spacing) < 0)
    {
        return 2;
    }
    if (this->set_notify_time(time) < 0)
    {
        return 2;
    }
    if (this->set_notify_size(width, height) < 0)
    {
        return 3;
    }
    if (this->set_notify_position(xpos, ypos) < 0)
    {
        return 4;
    }
    if (this->set_notify_color(background, foreground, opacity) < 0)
    {
        return 5;
    }
    if (this->set_notify_curve(curve) < 0)
    {
        return 6;
    }
    if (this->set_notify_margin(margin, margintop, marginbottom, marginleft,
                                marginright) < 0)
    {
        return 7;
    }

    return 0;
}

/**
 * @brief Display the notification bubble.
 * 
 * @details Align the widgets, add them to the main container, resize the
 *          widget, and move the notification bubble to the desired location.
 * 
 *          The reason why resizing is done here, and not when the notification
 *          bubble is built, is because aquiring the natural width and height
 *          measurements for the widget can only be done when it is realized,
 *          i.e. when it is shown. Then, move is done afterwards because if the
 *          notification is too long, it could potentially get cut-off on the
 *          side of the screen. As a result, knowing the width and height allow
 *          you to avoid this situation.
 * 
 * @return 0 on success.
 */
int notification::show(void)
{
    this->icon_.set_halign(Gtk::ALIGN_CENTER);
    this->icon_.set_valign(Gtk::ALIGN_CENTER);
    this->text_.set_halign(Gtk::ALIGN_CENTER);
    this->text_.set_valign(Gtk::ALIGN_CENTER);
    this->bubble_.set_halign(Gtk::ALIGN_CENTER);
    this->bubble_.set_valign(Gtk::ALIGN_CENTER);
    this->bubble_.pack_start(this->icon_);
    this->bubble_.pack_start(this->text_);
    this->add(this->bubble_);
    this->show_all_children();
    this->resize();
    this->reposition();
    return 0;
}

/**
 * @brief Resize the notification bubble to the desired size, if specified, or
 *        the preferred size, otherwise.
 * 
 * @details If the width and/or height are unset, determine the preferred width
 *          or height size of the widget. The curvature value is then added to
 *          the width and/or height as an additional padding.
 * 
 *          If the width and/or height are provided on the command line, use
 *          those values without modifying them.
 * 
 * @return 0 on success.
 */
int notification::resize(void)
{
    int junk;
    if (!this->width_)
    {
        this->bubble_.get_preferred_width(junk, this->width_);
        this->width_ += this->curve_;
    }
    if (!this->height_)
    {
        this->bubble_.get_preferred_height(junk, this->height_);
        this->height_ += this->curve_;
    }
    this->set_size_request(this->width_, this->height_);
    return 0;
}

/**
 * @brief Move the notification bubble to the desired position.
 * 
 * @details Add attributes into shared memory, determine the appropriate x-y
 *          positional values, and move the notification bubble.
 * 
 * @note Modify reposition to accurately determine data.y
 * @note Change 1920 to screen width.
 * 
 * @return 0 on success.
 */
int notification::reposition(void)
{
    int x = 0;
    int y = 0;
    int w = this->width_;
    int h = this->height_;
    int xpixels;
    int ypixels;
    if (!this->get_screen_resolution(xpixels, ypixels))
    {
        x = xpixels - (this->width_ + this->xpos_);
        y = this->ypos_;
    }
    struct SharedMemType data = {.id=getpid(), .time=time(0), .x=x, .y=y, .w=w,
                                 .h=h};
    AriaSharedMem::add(&data, 10);
    this->move(x, y);
    return 0;
}

/**
 * @brief Set the title and body text, as well as their respective fonts and
 *        sizes.
 * 
 * @details When setting the title and body, one can be set without the other,
 *          but if neither are set, that's an issue. So I get the return values
 *          from calling those functions and then I check their values. I don't
 *          put the function in the conditional because of the short-circuit
 *          feature of the && operator.
 * 
 * @param[in] title     Title of the notification bubble.
 * @param[in] body      Body of the notification bubble.
 * @param[in] font      Font for both the title and body.
 * @param[in] titlesize Font size of the title.
 * @param[in] bodysize  Font size of the body.
 * 
 * @return 0 on success. Any other values indicate an error.
 */
int notification::set_notify_title_and_body(std::string& title,
                                            std::string& body,
                                            std::string& font,
                                            std::string& titlesize,
                                            std::string& bodysize)
{
    int tstatus, bstatus;
    if (this->set_font(font) < 0)
    {
        return -1;
    }
    if (this->set_title_size(titlesize) < 0)
    {
        return -2;
    }
    if (this->set_body_size(bodysize) < 0)
    {
        return -3;
    }
    tstatus = this->set_title(title, font, titlesize);
    bstatus = this->set_body(body, font, bodysize);
    if ((tstatus < 0) && (bstatus < 0))
    {
        return -4;
    }
    return 0;
}

/**
 * @brief Set the title of the notification bubble.
 * 
 * @param[in] title The title of the notification bubble.
 * @param[in] font  The font to use for the title.
 * @param[in] size  The font size for the title.
 * 
 * @return See set_text(string, string, string).
 */
int notification::set_title(std::string& title, std::string& font, std::string& size)
{
    return this->set_text(title, font, size);
}

/**
 * @brief Set the title of the notification bubble.
 * 
 * @param[in] title The title of the notification bubble.
 * @param[in] font  The font to use for the title.
 * @param[in] size  The font size for the title.
 * 
 * @return See set_text(string, string, string).
 */
int notification::set_body(std::string& body, std::string& font, std::string& size)
{
    return this->set_text(body, font, size);
}

/**
 * @brief Generically create a label and add it to the notification bubble.
 * 
 * @details Create a label and configure it. Create a font description
 *          object. Find and replace any escaped backslashes. Add the label to
 *          the notification bubble text container.
 * 
 *          In the event that the text, font or size strings are empty, return
 *          indicating an error.
 * 
 * @param[in] text Text for the label.
 * @param[in] font Font for the text.
 * @param[in] size Font size for the text.
 * 
 * @return 0 on success. Any other value indicates an error.
 */
int notification::set_text(std::string text, std::string font, std::string size)
{
    if (text.empty())
    {
        return -1;
    }
    if (font.empty())
    {
        return -2;
    }
    if (size.empty())
    {
        return -3;
    }

    Gtk::Label* label = Gtk::manage(new Gtk::Label());
    Pango::FontDescription fd;
    fd.set_family(font);
    fd.set_size(std::stoi(size) * PANGO_SCALE);

    util::replace_all(text, "\\", "\n");
    label->set_use_markup(true);
    label->set_markup(text);
    label->set_line_wrap();
    label->override_font(fd);
    label->set_halign(Gtk::ALIGN_START);
    this->text_.pack_start(*label, Gtk::PACK_SHRINK);

    return 0;
}

/**
 * @brief Set the title size.
 * 
 * @param[in] size The title size.
 * 
 * @return See set_font_size(string, string).
 */
int notification::set_title_size(std::string& size)
{
    return this->set_font_size("title", size);
}

/**
 * @brief Set the body size.
 * 
 * @param[in] size The body size.
 * 
 * @return See set_font_size(string, string).
 */
int notification::set_body_size(std::string& size)
{
    return this->set_font_size("body", size);
}

/**
 * @brief Set the font to use for the text in the notification bubble.
 * 
 * @param[in] font The font to use for the text.
 * 
 * @return 0 on success. See set_from_config(string, string) for other return
 *         values.
 */
int notification::set_font(std::string& font)
{
    return (font.empty()) ? this->set_from_config("font", font) : 0;
}

/**
 * @brief Set the font size to use for the title or body.
 * 
 * @param[in] key  A string, "title" or "body", indicating to set the font size
 *                 for the title or body.
 * @param[in] size The font size to use.
 * 
 * @return 0 on success. -1 if an invalid key is supplied. See
 *         set_from_config(string, string) for any other return value.
 */
int notification::set_font_size(const std::string key, std::string& size)
{
    if ((key != "title") && (key != "body"))
    {
        return -1;
    }
    return (size.empty()) ? this->set_from_config((key+"-size").c_str(), size) : 0;
}

/**
 * @brief Set the notification icon.
 * 
 * @details Check to make sure the icon path exists. Read the config file to get
 *          the icon and text spacing, if it has not been set. Add the icon to
 *          the icon container.  Set notification bubble icon.
 * 
 * @param[in] path    The path to the icon.
 * @param[in] spacing The spacing, in pixels, between the icon and text.
 * 
 * @return 0 on success. Any other value to indicate error.
 */
int notification::set_notify_icon(std::string& path, std::string& spacing)
{
    struct stat statbuf;
    int s;
    if (path.empty())
    {
        return 0;
    }
    if (stat(path.c_str(), &statbuf) != 0)
    {
        return -1;
    }
    if (spacing.empty() && (this->set_from_config("icon-text-spacing",
                                                  spacing) < 0))
    {
        spacing = "0";
    }
    if ((s=std::stoi(spacing)) < 0)
    {
        return -2;
    }

    Gtk::Image* icon = Gtk::manage(new Gtk::Image(path));
    this->icon_.pack_start(*icon, Gtk::PACK_SHRINK);
    this->bubble_.set_spacing(s);

    return 0;
}

/**
 * @brief Set the amount of time after which the notification bubble will
 *        disappear.
 * 
 * @details If there is not time specified, read the config file and get the
 *          default value. Convert the value to an int, and use this to set the
 *          display timeout.
 * 
 * @param[in] time The amount of time, in seconds.
 * 
 * @return 0 on success. Any other value to indicate an error.
 */
int notification::set_notify_time(std::string& time)
{
    int t;
    if (time.empty() && (this->set_from_config("time", time) < 0))
    {
        return -1;
    }
    if ((t=std::stoi(time)) <= 0)
    {
        return -2;
    }
    Glib::signal_timeout().connect_seconds_once(
        sigc::bind<int>(sigc::ptr_fun(&notification::cleanup), 0), t);
    return 0;
}

/**
 * @brief Set the notification bubble size (width x height).
 * 
 * @details This sets the width and height that are entered by the user. If no
 *          values are entered, then the preferred width and height are
 *          calculated in show().
 * 
 * @note Have bounds check, and get screen size here. Check if screen size is set first though.
 * 
 * @param[in] width  The width of the notification bubble, in pixels.
 * @param[in] height The height of the notification bubble, in pixels.
 * 
 * @return 0 on success.
 */
int notification::set_notify_size(std::string& width, std::string& height)
{
    if (!width.empty())
    {
        this->width_ = std::stoi(width);
    }
    if (!height.empty())
    {
        this->height_ = std::stoi(height);
    }
    return 0;
}

/**
 * @brief Set the notification bubble position on the screen.
 * 
 * @details This sets the X and Y position that is entered by the user. If no
 *          values are entered, the the X and Y positions are not set.
 * 
 * @note Have bounds check, and get screen size here. Check if screen size is
 *       set first though.
 * 
 * @param[in] xpos X-position on the screen.
 * @param[in] ypos Y-position on the screen.
 * 
 * @return 0 on success.
 */
int notification::set_notify_position(std::string& xpos, std::string& ypos)
{
    if (!xpos.empty())
    {
        this->xpos_ = std::stoi(xpos);
    }
    if (!ypos.empty())
    {
        this->ypos_ = std::stoi(ypos);
    }
    return 0;
}

/**
 * @brief Set the notification background, foreground, and opacity.
 * 
 * @param[in] background The color of the notification bubble.
 * @param[in] foreground The color of the text.
 * @param[in] opacity    The opacity of the notification bubble.
 * 
 * @return 0 on success. Any other value is an error.
 */
int notification::set_notify_color(std::string& background,
                                   std::string& foreground,
                                   std::string& opacity)
{
    if (this->set_background(background) < 0)
    {
        return -1;
    }
    if (this->set_foreground(foreground) < 0)
    {
        return -2;
    }
    if (this->set_opacity(opacity) < 0)
    {
        return -3;
    }
    return 0;
}

/**
 * @brief Set the color of the notification bubble.
 * 
 * @param[in] color The color of the notification bubble.
 * 
 * @return See set_color(string, string).
 */
int notification::set_background(std::string& color)
{
    return this->set_color("background", color);
}

/**
 * @brief Set the text color of the notification bubble.
 * 
 * @param[in] color The color of the notification bubble.
 * 
 * @return See set_color(string, string).
 */
int notification::set_foreground(std::string& color)
{
    return this->set_color("foreground", color);
}

/**
 * @brief Set the foreground or background color.
 * 
 * @details If color is empty and can't be read from the config file, this is an
 *          error. If key is not "background" or "foreground" this is an
 *          error. Otherwise, set the respective colors.
 * 
 * @param[in] key   The key to look for in the config file, in the event that
 *                  color is not specified.
 * @param[in] color The background or foreground color to set.
 * 
 * @return 0 on success. Any other value indicates an error.
 */
int notification::set_color(const std::string key, std::string& color)
{
    auto flag = Gtk::STATE_FLAG_NORMAL;
    if (color.empty() && (this->set_from_config(key, color) < 0))
    {
        return -1;
    }
    color = this->fix_color(color);

    if (key == "background")
    {
        this->background_.set(color);
        this->override_background_color(this->background_, flag);
    }
    else if (key == "foreground")
    {
        Gdk::RGBA rgba(color);
        this->override_color(rgba, flag);
    }
    else
    {
        return -2;
    }
    return 0;
}

/**
 * @brief Set the opacity of the notification bubble.
 * 
 * @details If opacity is not specified and this is unable to read the value
 *          from the config file, this is an error. When converting the string
 *          to a double, if the resultant value is not between 0 and 1, this is
 *          an error. Otherwise, set the opacity.
 * 
 * @param[in] opacity The opacity of the notification bubble.
 * 
 * @return 0 on success. Any other value indicates an error.
 */
int notification::set_opacity(std::string& opacity)
{
    if (opacity.empty() && (this->set_from_config("opacity", opacity) < 0))
    {
        return -1;
    }
    double o = std::stod(opacity);
    int i = (int)o;
    if ((i != 0) && (i != 1))
    {
        return -2;
    }
    this->background_.set_alpha(o);
    return 0;
}

/**
 * @brief Set the curvature of the corners of the notification bubble.
 * 
 * @details If the input is not specified and this is unable to read it from the
 *          config file, this is an error. If this is unable to convert the
 *          string to a valid (greater than 0) integer, this is also an
 *          error. Otherwise set the curvature.
 * 
 * @param[in] curve The curvature of the corners (in pixels).
 * 
 * @return 0 on success. Any other value indicates an error.
 */
int notification::set_notify_curve(std::string& curve)
{
    int c;
    if (curve.empty() && (this->set_from_config("curve", curve) < 0))
    {
        return -1;
    }
    if ((c=std::stoi(curve)) < 0)
    {
        return -2;
    }
    this->curve_ = c;
    return 0;
}

/**
 * @brief Set the margin of the notification bubble.
 * 
 * @details If the main margin is not empty, set all margins to this
 *          value. Otherwise, set the other margins individually, "margin-top",
 *          "margin-bottom", etc., from the config file.
 * 
 * @param[in] margin  The main margin value, in pixels. If this is set, all
 *                    other margins are set to this value.
 * @param[in] mtop    The margin from the top, in pixels.
 * @param[in] mbottom The margin from the bottom, in pixels.
 * @param[in] mleft   The margin from the left, in pixels.
 * @param[in] mright  The margin from the right, in pixels.
 * 
 * @return 0 on success. Any other value indicates an error.
 */
int notification::set_notify_margin(std::string& margin, std::string& mtop,
                                    std::string& mbottom, std::string& mleft,
                                    std::string& mright)
{
    int mall, mt, mb, ml, mr;
    if (!margin.empty())
    {
        if ((mall=std::stoi(margin)) < 0)
        {
            return -1;
        }
        mt = mall;
        mb = mall;
        ml = mall;
        mr = mall;
    }
    else
    {
        if (mtop.empty() && (this->set_from_config("margin-top", mtop) < 0))
        {
            return -2;
        }
        if (mbottom.empty() && (this->set_from_config("margin-bottom",
                                                      mbottom) < 0))
        {
            return -3;
        }
        if (mleft.empty() && (this->set_from_config("margin-left", mleft) < 0))
        {
            return -4;
        }
        if (mright.empty() && (this->set_from_config("margin-right", mright) < 0))
        {
            return -5;
        }
        if (((mt=std::stoi(mtop)) < 0) || ((mb=std::stoi(mbottom)) < 0)
            || ((ml=std::stoi(mleft)) < 0) || ((mr=std::stoi(mright)) < 0))
        {
            return -6;
        }
    }

    this->bubble_.set_margin_top(mt);
    this->bubble_.set_margin_bottom(mb);
    this->bubble_.set_margin_start(ml);
    this->bubble_.set_margin_end(mr);

    return 0;
}

/**
 * @brief Search the config file for the key, and set value to the one found in
 *        the config file.
 * 
 * @param[in] key   The string to search for in the config file.
 * @param[in] value The value to populate with the one found in the config file.
 * 
 * @return 0 on success. Any other value indicates an error.
 */
int notification::set_from_config(const std::string key, std::string& value)
{
    if (key.empty()) {
        return -1;
    }
    value = config::read(key.c_str());
    if (value.empty()) {
        return -2;
    }
    return 0;
}

/**
 * @brief Parse the color string and convert it to a '#123456' string if the
 *        input is a hex string.
 * 
 * @param[in] color The color hex string.
 * 
 * @return The color string.
 */
std::string notification::fix_color(std::string& color)
{
    if ((color.substr(0, 2) == "0x") || (color.substr(0, 2) == "0X")) {
        color.erase(0, 2);
    }
    if (this->is_hex_color(color)) {
        if (color[0] != '#') {
            color.insert(0, 1, '#');
        }
    }
    return color;
}

/**
 * @brief Determine the screen resolution for monitor 0 from 
 * 
 * @param[out] width  The width of the monitor.
 * @param[out] height The height of the monitor.
 * 
 * @return 0 on success, and width and height will be populated. If any
 *         other value is returned, width and height will be in an
 *         undetermined state.
 */
int notification::get_screen_resolution(int& width, int& height)
{
    char* display;
    Display* xopen;
    XRRScreenResources* screen;
    if (!(display=getenv("DISPLAY")))
    {
        fprintf(stderr, "Get DISPLAY environment variable error.\n");
        return -1;
    }
    if (!(xopen=XOpenDisplay(display)))
    {
        fprintf(stderr, "X Open Display error.\n");
        return -2;
    }
    auto window = DefaultRootWindow(xopen);
    if (!(screen=XRRGetScreenResources(xopen, window)))
    {
        fprintf(stderr, "XRR Get Screen Resources error.\n");
        return -3;
    }

    /* Set screen dimensions for the monitor at x=0 */
    int          num = screen->ncrtc;
    int          i;
    XRRCrtcInfo* info;
    for (i=0; i < num; i++)
    {
        info = XRRGetCrtcInfo(xopen, screen, screen->crtcs[i]);
        if (info->x == 0)
        {
            width  = info->width;
            height = info->height;
            XRRFreeCrtcInfo(info);
            break;
        }
        XRRFreeCrtcInfo(info);
    }
    XRRFreeScreenResources(screen);

    return 0;
}

/**
 * @brief Check if the color string is in a hex format.
 * 
 * @param[in] color The color string to check.
 * 
 * @return True if the input is a color string, and false otherwise.
 */
bool notification::is_hex_color(std::string& color)
{
    int start;
    if (color[0] == '#') {
        return true;
    }
    if (color.length() != 6) {
        return false;
    }
    start = 0;
    if ((color.substr(0, 2) == "0x") || (color.substr(0, 2) == "0X")) {
        start = 2;
    }
    return (color.find_first_not_of("0123456789ABCDEFabcdef", start) == std::string::npos);
}

/**
 * @brief Cleanup any memory mapped data and gracefully shutdown program.
 * 
 * @param[in] sig The signal that was captured, to cause the cleanup function to
 *                run.
 */
void notification::cleanup(int sig)
{
    AriaSharedMem::remove();
    exit(sig);
}

/**
 * @brief Draw the notification bubble.
 *
 * @param[in] cr Cairo drawing context.
 *
 * @return True or false.
 */
bool notification::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Glib::RefPtr<Gdk::Screen> screen = this->get_screen();
    double width  = this->width_;
    double height = this->height_;
    double curve  = this->curve_;
    double deg    = M_PI / 180.0;
    double red    = this->background_.get_red();
    double green  = this->background_.get_green();
    double blue   = this->background_.get_blue();
    double alpha  = this->background_.get_alpha();
    cr->save();
    cr->set_line_width(0);
    cr->begin_new_path();
    cr->arc(width-curve, curve,        curve, -90*deg,   0*deg);
    cr->arc(width-curve, height-curve, curve,   0*deg,  90*deg);
    cr->arc(curve,       height-curve, curve,  90*deg, 180*deg);
    cr->arc(curve,       curve,        curve, 180*deg, 270*deg);
    cr->close_path();
    if (screen->is_composited()) {
        cr->set_source_rgba(red, green, blue, alpha);
    } else {
        cr->set_source_rgb(red, green, blue);
    }
    cr->fill_preserve();
    cr->stroke();
    return Gtk::Window::on_draw(cr);
}

/**
 * @brief Set the visual that should be used for the gobj(). Not really sure
 *        what this does.
 *
 * @param[in] previous_screen The previous screen.
 */
void notification::on_screen_changed(const Glib::RefPtr<Gdk::Screen>& previous_screen)
{
    auto screen = get_screen();
    auto visual = screen->get_rgba_visual();
    if (!visual) {
        std::cout << "Your screen does not support alpha channels!" << std::endl;
    }
    gtk_widget_set_visual(GTK_WIDGET(gobj()), visual->gobj());
}

ARIA_NAMESPACE_END
