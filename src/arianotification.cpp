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
    std::string title = cli.get("--title");
    std::string body = cli.get("--body");
    std::string font = cli.get("--font");
    std::string titlesize = cli.get("--title-size");
    std::string bodysize = cli.get("--body-size");
    std::string background = cli.get("--background");
    std::string foreground = cli.get("--foreground");
    std::string opacity = cli.get("--opacity");
    std::string margin = cli.get("--margin");
    std::string time = cli.get("--time");
    std::string width = cli.get("--width");
    std::string height = cli.get("--height");
    std::string xpos = cli.get("--xpos");
    std::string ypos = cli.get("--ypos");

    if ((this->set_title(title, font, titlesize) < 0)
        && (this->set_body(body, font, bodysize) < 0))
    {
        return 1;
    }
    if (this->set_background(background) < 0) {
        return 2;
    }
    if (this->set_foreground(foreground) < 0) {
        return 3;
    }
    if (this->set_margin(margin) < 0) {
        return 4;
    }
    if (this->set_opacity(opacity) < 0) {
        return 5;
    }
    if (this->set_time(time) < 0) {
        return 6;
    }
    if (this->set_size(width, height) < 0) {
        return 7;
    }
    if (this->set_position(xpos, ypos) < 0) {
        return 8;
    }

    return 0;
}

/**
 * Display the notification bubble and size it accordingly
 */
int notification::show(void)
{
    // m_bubble.pack_start(this->m_icon);
    m_bubble.pack_start(this->m_text);
    this->add(this->m_bubble);
    this->show_all_children();

    // this->m_text.set_y_align(0.5);

    this->resize();
    this->reposition();
    return 0;
}

/**
 * Resize the notification bubble to the desired width and height
 */
int notification::resize(void)
{
    int w = this->m_width;
    int h = this->m_height;
    int tmp;

    if (!this->m_width) {
        this->get_size(w, tmp);
    }
    if (!this->m_height) {
        this->get_size(tmp, h);
    }

    this->set_default_size(w, h);
    this->m_width = w;
    this->m_height = h;
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
    double deg = M_PI / 180.0;
    double width = this->m_width;
    double height = this->m_height;

    // Go with natural width and height
    int mwidth;
    int nwidth;
    int mheight;
    int nheight;
    this->m_bubble.get_preferred_width(mwidth, nwidth);
    this->m_bubble.get_preferred_height(mheight, nheight);

    printf("TextWidth  : %d %d\n", mwidth, nwidth);
    printf("TextHeight : %d %d\n", mheight, nheight);

    Gtk::Allocation allocation = get_allocation();
    if (!this->m_width) {
        width = 1.0*allocation.get_width();
    }
    if (!this->m_height) {
        height = 1.0*allocation.get_height();
    }

    printf("Width  : %lf\n", width);
    printf("Height : %lf\n", height);
    printf("Curve  : %lf\n", curve);

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
 * Set font
 */
int notification::set_font(std::string& font)
{
    if (font.empty()) {
        font = config::read("font");
    }
    return (font.empty()) ? -1 : 0;
}

/**
 * Set font size
 */
int notification::set_font_size(const std::string region, std::string& size)
{
    if (region.empty()) {
        return -1;
    }
    else if ((region != "title") && (region != "body")) {
        return -2;
    }
    else {
        size = config::read((region+"-size").c_str());
    }
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
 * Set the title
 */
int notification::set_title(std::string& title, std::string& font, std::string& size)
{
    if (this->set_font(font) < 0) {
        return -2;
    }
    if (this->set_title_size(size) < 0) {
        return -3;
    }
    return this->set_text(title, font, size);
}

/**
 * Set the body
 */
int notification::set_body(std::string& body, std::string& font, std::string& size)
{
    if (this->set_font(font) < 0) {
        return -2;
    }
    if (this->set_body_size(size) < 0) {
        return -3;
    }
    return this->set_text(body, font, size);
}

/**
 * Set foreground/background color
 */
int notification::set_color(const std::string region, std::string& color)
{
    if (color.empty()) {
        color = config::read(region.c_str());
        if (color.empty()) {
            return -1;
        }
    }

    color = this->fix_color(color);

    Gdk::RGBA attr(color);
    if (region == "background") {
        this->m_background = color;
        this->override_background_color(attr, Gtk::STATE_FLAG_NORMAL);
    }
    else if (region == "foreground") {
        this->override_color(attr, Gtk::STATE_FLAG_NORMAL);
    }
    else {
        return -2;
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
 * Set the margin
 */
int notification::set_margin(std::string& margin)
{
    if (margin.empty()) {
        margin = config::read("margin");
        if (margin.empty()) {
            return -1;
        }
    }

    int m = std::stoi(margin);
    if (m < 0) {
        return -2;
    }

    m_bubble.set_margin_top(m);
    m_bubble.set_margin_bottom(m);
    m_bubble.set_margin_start(m);
    m_bubble.set_margin_end(m);
    return 0;
}

/**
 * Set opacity of window
 * To-do: Check if input is int or double
 */
int notification::set_opacity(std::string& opacity)
{
    if (opacity.empty()) {
        opacity = config::read("opacity");
        if (opacity.empty()) {
            return -1;
        }
    }
    this->m_opacity = opacity;
    // Gtk::Window::set_opacity(std::stod(opacity));
    return 0;
}

/**
 * Set the time at which afterwards, the notification bubble will close
 */
int notification::set_time(std::string& time)
{
    if (time.empty()) {
        time = config::read("time");
        if (time.empty()) {
            return -1;
        }
    }

    int t = std::stoi(time);
    if (t <= 0) {
        return -2;
    }

    Glib::signal_timeout().connect_seconds_once(
        sigc::bind<int>(sigc::ptr_fun(&cleanup), 0), t);
    return 0;
}

/**
 * Set the notification bubble size
 */
int notification::set_size(std::string& width, std::string& height)
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
 */
int notification::set_position(std::string& xpos, std::string& ypos)
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

ARIA_NAMESPACE_END
