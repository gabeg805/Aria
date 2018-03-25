/* -----------------------------------------------------------------------------
 * 
 * Name:    arianotification.cpp
 * Class:   <notification>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: The Aria Notification Bubble.
 * 
 * Notes: None.
 * 
 * -----------------------------------------------------------------------------
 */

/* Includes */
#include "arianotification.hpp"
#include "AriaSharedMem.h"
#include "ariacommandline.hpp"
#include "ariaconf.hpp"
#include <gtkmm.h>
#include <gdkmm.h>
#include <time.h>
#include <unistd.h>
#include <pangomm/fontdescription.h>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>

ARIA_NAMESPACE

/**
 * Construct the notification window and container.
 */
notification::notification() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    m_bubble(Gtk::ORIENTATION_HORIZONTAL),
    m_icon(Gtk::ORIENTATION_VERTICAL),
    m_text(Gtk::ORIENTATION_VERTICAL)
{
    this->m_background = "";
    this->m_opacity = "";
    this->m_width = 0;
    this->m_height = 0;
    this->m_xpos = 0;
    this->m_ypos = 0;

    this->m_curve = 0;
    this->m_margin_top = 0;
    this->m_margin_bottom = 0;
    this->m_margin_left = 0;
    this->m_margin_right = 0;

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
 * Build the notification bubble
 */
int notification::build(commandline::interface& cli)
{
    std::string title        = cli.get("--title");
    std::string body         = cli.get("--body");
    std::string font         = cli.get("--font");
    std::string titlesize    = cli.get("--title-size");
    std::string bodysize     = cli.get("--body-size");
    std::string time         = cli.get("--time");
    std::string xpos         = cli.get("--xpos");
    std::string ypos         = cli.get("--ypos");
    std::string width        = cli.get("--width");
    std::string height       = cli.get("--height");
    std::string background   = cli.get("--background");
    std::string foreground   = cli.get("--foreground");
    std::string opacity      = cli.get("--opacity");
    std::string curve        = cli.get("--curve");
    std::string margin       = cli.get("--margin");
    std::string margintop    = cli.get("--margin-top");
    std::string marginbottom = cli.get("--margin-bottom");
    std::string marginleft   = cli.get("--margin-left");
    std::string marginright  = cli.get("--margin-right");

    if (this->set_notify_title_and_body(title, body, font, titlesize,
                                        bodysize) < 0)
    {
        printf("AHHHHHH\n");
        return 1;
    }
    if (this->set_notify_time(time) < 0) {
        return 2;
    }
    if (this->set_notify_size(width, height) < 0) {
        return 3;
    }
    if (this->set_notify_position(xpos, ypos) < 0) {
        return 4;
    }
    if (this->set_notify_color(background, foreground, opacity) < 0) {
        return 5;
    }
    if (this->set_notify_curve(curve) < 0) {
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
 * Display the notification bubble and size it accordingly
 * To-do: Move set_valign elsewhere
 */
int notification::show(void)
{
    this->m_text.set_valign(Gtk::ALIGN_CENTER);
    // m_bubble.pack_start(this->m_icon);
    m_bubble.pack_start(this->m_text);
    this->add(this->m_bubble);
    this->show_all_children();
    this->resize();
    this->reposition();
    return 0;
}

/**
 * Resize the notification bubble to the desired width and height
 * To-do: Add curve to padding
 */
int notification::resize(void)
{
    const int padding = 10;
    int junk;
    if (!this->m_width) {
        this->m_bubble.get_preferred_width(junk, this->m_width);
        this->m_width += padding;
    }
    if (!this->m_height) {
        this->m_bubble.get_preferred_height(junk, this->m_height);
        this->m_height += padding;
    }
    this->set_size_request(this->m_width, this->m_height);
    return 0;
}

/**
 * Move the notification bubble to the desired position.
 * To-do: Change 1920 to screen width
 */
int notification::reposition(void)
{
    struct SharedMemType data = {.id   = getpid(),
                                 .time = time(0),
                                 .x    = this->m_xpos,
                                 .y    = this->m_ypos,
                                 .w    = this->m_width,
                                 .h    = this->m_height};
    AriaSharedMem::add(&data, 10);
    data.x = (1920 - data.w) - data.x;
    this->move(data.x, data.y);
    return 0;
}

/**
 * Set title and body text
 */
int notification::set_notify_title_and_body(std::string& title,
                                            std::string& body,
                                            std::string& font,
                                            std::string& titlesize,
                                            std::string& bodysize)
{
    if (this->set_font(font) < 0) {
        return -1;
    }
    if (this->set_title_size(titlesize) < 0) {
        return -2;
    }
    if (this->set_body_size(bodysize) < 0) {
        return -3;
    }
    if ((this->set_title(title, font, titlesize) < 0)
        && (this->set_body(body, font, bodysize) < 0))
    {
        return -4;
    }
    return 0;
}

/**
 * Set the title
 */
int notification::set_title(std::string& title, std::string& font, std::string& size)
{
    return this->set_text(title, font, size);
}

/**
 * Set the body
 */
int notification::set_body(std::string& body, std::string& font, std::string& size)
{
    return this->set_text(body, font, size);
}

/**
 * Set the font family, font size, and text for the given notification field.
 */
int notification::set_text(std::string text, std::string font, std::string size)
{
    if (text.empty()) {
        return -1;
    }
    if (font.empty()) {
        return -2;
    }
    if (size.empty()) {
        return -3;
    }

    Gtk::Label *label = Gtk::manage(new Gtk::Label());
    Pango::FontDescription fd;
    fd.set_family(font);
    fd.set_size(std::stoi(size) * PANGO_SCALE);

    size_t length = 2;
    for(size_t i=0; (i = text.find("\\", i)) != std::string::npos; ) {
        text.replace(i, length, "\n");
        i += length;
    }

    label->set_use_markup(true);
    label->set_markup(text);
    label->set_line_wrap();
    label->override_font(fd);
    m_text.pack_start(*label, Gtk::PACK_SHRINK);

    return 0;
}

/**
 * Set title size
 */
int notification::set_title_size(std::string& size)
{
    return this->set_font_size("title", size);
}

/**
 * Set body size
 */
int notification::set_body_size(std::string& size)
{
    return this->set_font_size("body", size);
}

/**
 * Set font
 */
int notification::set_font(std::string& font)
{
    return (font.empty()) ? this->set_from_config("font", font) : 0;
}

/**
 * Set font size
 */
int notification::set_font_size(const std::string key, std::string& size)
{
    if ((key != "title") && (key != "body")) {
        return -1;
    }
    return (size.empty()) ? this->set_from_config((key+"-size").c_str(), size) : 0;
}

/**
 * Set the time at which afterwards, the notification bubble will close
 */
int notification::set_notify_time(std::string& time)
{
    int t;
    if (time.empty() && (this->set_from_config("time", time) < 0)) {
        return -1;
    }
    if ((t=std::stoi(time)) <= 0) {
        return -2;
    }
    Glib::signal_timeout().connect_seconds_once(
        sigc::bind<int>(sigc::ptr_fun(&notification::cleanup), 0), t);
    return 0;
}

/**
 * Set the notification bubble size
 * To-do: Have bounds check, and get screen size here. Check if screen size is set first though.
 */
int notification::set_notify_size(std::string& width, std::string& height)
{
    if (!width.empty()) {
        this->m_width = std::stoi(width);
    }
    if (!height.empty()) {
        this->m_height = std::stoi(height);
    }
    return 0;
}

/**
 * Set the notification bubble position.
 * To-do: Have bounds check, and get screen size here. Check if screen size is set first though.
 */
int notification::set_notify_position(std::string& xpos, std::string& ypos)
{
    if (!xpos.empty()) {
        this->m_xpos = std::stoi(xpos);
    }
    if (!ypos.empty()) {
        this->m_ypos = std::stoi(ypos);
    }
    return 0;
}

/**
 * Set notification color attributes, including opacity.
 */
int notification::set_notify_color(std::string& background,
                                   std::string& foreground,
                                   std::string& opacity)
{
    printf("Background : %s\n", background.c_str());
    printf("Foreground : %s\n", foreground.c_str());
    printf("Opacity    : %s\n", opacity.c_str());

    if (this->set_background(background) < 0) {
        return -1;
    }
    if (this->set_foreground(foreground) < 0) {
        return -2;
    }
    if (this->set_opacity(opacity) < 0) {
        return -3;
    }
    return 0;
}

/**
 * Set the background color
 */
int notification::set_background(std::string& color)
{
    return this->set_color("background", color);
}

/**
 * Set the foreground color -- this is the font color
 */
int notification::set_foreground(std::string& color)
{
    return this->set_color("foreground", color);
}

/**
 * Set foreground/background color
 */
int notification::set_color(const std::string key, std::string& color)
{
    if (color.empty() && (this->set_from_config(key, color) < 0)) {
        return -1;
    }
    color = this->fix_color(color);

    Gdk::RGBA attr(color);
    if (key == "background") {
        this->m_background = color;
        this->override_background_color(attr, Gtk::STATE_FLAG_NORMAL);
    }
    else if (key == "foreground") {
        this->override_color(attr, Gtk::STATE_FLAG_NORMAL);
    }
    else {
        return -2;
    }
    return 0;
}

/**
 * Set opacity of window
 * To-do: Check if input is int or double
 */
int notification::set_opacity(std::string& opacity)
{
    if (opacity.empty() && (this->set_from_config("opacity", opacity) < 0)) {
        return -1;
    }
    this->m_opacity = opacity;
    // Gtk::Window::set_opacity(std::stod(opacity));
    return 0;
}

/**
 * Set notification bubble curvature around the corners.
 */
int notification::set_notify_curve(std::string& curve)
{
    int c;
    if (curve.empty() && (this->set_from_config("curve", curve) < 0)) {
        return -1;
    }
    if ((c=std::stoi(curve)) < 0) {
        return -2;
    }
    this->m_curve = c;
    return 0;
}

/**
 * Set the margin
 */
int notification::set_notify_margin(std::string& margin, std::string& mtop,
                                    std::string& mbottom, std::string& mleft,
                                    std::string& mright)
{
    int mall, mt, mb, ml, mr;
    if (!margin.empty()) {
        if ((mall=std::stoi(margin)) < 0) {
            return -1;
        }
        mt = mall;
        mb = mall;
        ml = mall;
        mr = mall;
    }
    else {
        if (mtop.empty() && (this->set_from_config("margin-top", mtop) < 0)) {
            return -2;
        }
        if (mbottom.empty() && (this->set_from_config("margin-bottom",
                                                      mbottom) < 0))
        {
            return -3;
        }
        if (mleft.empty() && (this->set_from_config("margin-left", mleft) < 0)) {
            return -4;
        }
        if (mright.empty() && (this->set_from_config("margin-right", mright) < 0)) {
            return -5;
        }
        if (((mt=std::stoi(mtop)) < 0) || ((mb=std::stoi(mbottom)) < 0)
            || ((ml=std::stoi(mleft)) < 0) || ((mr=std::stoi(mright)) < 0))
        {
            return -6;
        }
    }

    m_bubble.set_margin_top(mt);
    m_bubble.set_margin_bottom(mb);
    m_bubble.set_margin_start(ml);
    m_bubble.set_margin_end(mr);
    return 0;
}

/**
 * Set value from configuration file
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
 * Fix color string in case entered strangely
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
 * Check if color string is in hex
 */
bool notification::is_hex_color(std::string& color)
{
    if (color[0] == '#') {
        return true;
    }
    if (color.length() != 6) {
        return false;
    }

    int start = 0;
    if ((color.substr(0, 2) == "0x") || (color.substr(0, 2) == "0X")) {
        start = 2;
    }
    return (color.find_first_not_of("0123456789ABCDEFabcdef", start) == std::string::npos);
}





/**
 * Cleanup any memory mapped data and gracefully shutdown program
 */
void notification::cleanup(int sig)
{
    AriaSharedMem::remove();
    exit(sig);
}


bool notification::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Glib::RefPtr<Gdk::Screen> screen = this->get_screen();
    double curve = 20;
    double width = this->m_width;
    double height = this->m_height;
    double deg = M_PI / 180.0;

    printf("Width  : %lf\n", width);
    printf("Height : %lf\n", height);
    printf("Curve  : %lf\n", curve);
    printf("Back   : %s\n", this->m_background.c_str());

    int r;
    int g;
    int b;
    std::istringstream(this->m_background.substr(1, 2)) >> std::hex >> r;
    std::istringstream(this->m_background.substr(3, 2)) >> std::hex >> g;
    std::istringstream(this->m_background.substr(5, 2)) >> std::hex >> b;

    cr->save();
    cr->set_line_width(0);
    cr->begin_new_path();
    cr->arc(width-curve, curve,        curve, -90*deg,   0*deg);
    cr->arc(width-curve, height-curve, curve,   0*deg,  90*deg);
    cr->arc(curve,       height-curve, curve,  90*deg, 180*deg);
    cr->arc(curve,       curve,        curve, 180*deg, 270*deg);
    cr->close_path();
    if (screen->is_composited()) {
        cr->set_source_rgba(r/255.0, g/255.0, b/255.0, 0.5);
    } else {
        cr->set_source_rgb(r/255.0, g/255.0, b/255.0);
    }
    cr->fill_preserve();
    cr->stroke();

    return Gtk::Window::on_draw(cr);
}

/**
 * Checks to see if the display supports alpha channels
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
