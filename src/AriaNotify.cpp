/* -----------------------------------------------------------------------------
 * 
 * Name:    AriaNotify.cc
 * Class:   <AriaNotify>
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
#include "AriaNotify.h"
#include "AriaSharedMem.h"
#include "commandline.h"
#include <time.h>
#include <unistd.h>
#include <gtkmm.h>
#include <gdkmm.h>
#include <pangomm/fontdescription.h>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>

/**
 * Construct the notification window and container.
 */
AriaNotify::AriaNotify() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    bubble(Gtk::ORIENTATION_VERTICAL)
{
    this->m_background = "";
    this->m_opacity = "";
    this->m_width = 0;
    this->m_height = 0;
    this->m_xpos = 0;
    this->m_ypos = 0;

    this->set_decorated(false);
    this->set_app_paintable(true);
    this->signal_draw().connect(sigc::mem_fun(*this, &AriaNotify::on_draw));
    this->signal_screen_changed().connect(sigc::mem_fun(*this, &AriaNotify::on_screen_changed));
    this->on_screen_changed(get_screen());

    std::signal(SIGINT,  cleanup);
    std::signal(SIGQUIT, cleanup);
    std::signal(SIGTERM, cleanup);
}

bool AriaNotify::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Glib::RefPtr<Gdk::Screen> screen = this->get_screen();
    double curve = 20;
    double deg = M_PI / 180.0;
    double width = this->m_width;
    double height = this->m_height;

    Gtk::Allocation allocation = get_allocation();
    if (!this->m_width) {
        width = 1.0*allocation.get_width();
    }
    if (!this->m_height) {
        height = 1.0*allocation.get_height();
    }

    int r;
    int g;
    int b;
    std::istringstream(this->m_background.substr(0, 2)) >> std::hex >> r;
    std::istringstream(this->m_background.substr(2, 2)) >> std::hex >> g;
    std::istringstream(this->m_background.substr(4, 2)) >> std::hex >> b;

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
void AriaNotify::on_screen_changed(const Glib::RefPtr<Gdk::Screen>& previous_screen)
{
    auto screen = get_screen();
    auto visual = screen->get_rgba_visual();
    if (!visual) {
        std::cout << "Your screen does not support alpha channels!" << std::endl;
    }
    gtk_widget_set_visual(GTK_WIDGET(gobj()), visual->gobj());
}

std::string get_val(commandline::values optvalues, std::string opt)
{
    if (optvalues.find(opt) != optvalues.end()) {
        return optvalues.find(opt)->second;
    }
    return "";
}

/**
 * Build the notification bubble
 */
int AriaNotify::build(commandline::values optvalues)
{
    std::string title = get_val(optvalues, "title");
    std::string body = get_val(optvalues, "body");
    std::string font = get_val(optvalues, "font");
    std::string titlesize = get_val(optvalues, "title-size");
    std::string bodysize = get_val(optvalues, "body-size");
    std::string background = get_val(optvalues, "background");
    std::string foreground = get_val(optvalues, "foreground");
    std::string opacity = get_val(optvalues, "opacity");
    std::string margin = get_val(optvalues, "margin");
    std::string time = get_val(optvalues, "time");
    std::string width = get_val(optvalues, "width");
    std::string height = get_val(optvalues, "height");
    std::string xpos = get_val(optvalues, "xpos");
    std::string ypos = get_val(optvalues, "ypos");

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
int AriaNotify::show(void)
{
    this->add(bubble);
    this->show_all_children();
    this->resize();
    this->reposition();
    return 0;
}

/**
 * Set the title
 */
int AriaNotify::set_title(std::string title, std::string font, std::string size)
{
    return this->set_text(title, font, size);
}

/**
 * Set the body
 */
int AriaNotify::set_body(std::string body, std::string font, std::string size)
{
    return this->set_text(body, font, size);
}

/**
 * Set the font family, font size, and text for the given notification field.
 */
int AriaNotify::set_text(std::string text, std::string font, std::string size)
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
    for(size_t i=0; (i = text.find("\\", i)) != std::string::npos; )
    {
        text.replace(i, length, "\n");
        i += length;
    }

    // void find_and_replace(string& source, string const& find, string const& replace)
    // {
    //     for(string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
    //     {
    //         source.replace(i, find.length(), replace);
    //         i += replace.length();
    //     }
    // }
    std::cout << "Text : " << text << std::endl;

    label->set_use_markup(true);
    label->set_markup(text);
    label->set_line_wrap();
    label->override_font(fd);
    bubble.pack_start(*label, Gtk::PACK_SHRINK);
    return 0;
}

/**
 * Set the background color
 */
int AriaNotify::set_background(std::string color)
{
    this->m_background = color;
    if (color.empty()) {
        return -1;
    }
    if (color[0] != '#') {
        color.insert(0, 1, '#');
    }

    Gdk::RGBA background(color);
    this->override_background_color(background, Gtk::STATE_FLAG_NORMAL);
    return 0;
}

/**
 * Set the foreground color -- this is the font color
 */
int AriaNotify::set_foreground(std::string color)
{
    if (color.empty()) {
        return -1;
    }
    if (color[0] != '#') {
        color.insert(0, 1, '#');
    }

    Gdk::RGBA foreground(color);
    this->override_color(foreground, Gtk::STATE_FLAG_NORMAL);
    return 0;
}

/**
 * Set the margin
 */
int AriaNotify::set_margin(std::string margin)
{
    int m = std::stoi(margin);
    if (m < 0) {
        return -1;
    }

    bubble.set_margin_top(m);
    bubble.set_margin_bottom(m);
    bubble.set_margin_start(m);
    bubble.set_margin_end(m);
    return 0;
}

/**
 * Set opacity of window
 */
int AriaNotify::set_opacity(std::string opacity)
{
    this->m_opacity = opacity;
    if (opacity.empty()) {
        return -1;
    }
    // Gtk::Window::set_opacity(std::stod(opacity));
    return 0;
}

/**
 * Set the time at which afterwards, the notification bubble will close
 */
int AriaNotify::set_time(std::string time)
{
    int t = std::stoi(time);
    if (t <= 0) {
        return -1;
    }

    Glib::signal_timeout().connect_seconds_once(
        sigc::bind<int>(sigc::ptr_fun(&cleanup), 0), t);
    return 0;
}

/**
 * Set the notification bubble size
 */
int AriaNotify::set_size(std::string width, std::string height)
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
int AriaNotify::set_position(std::string xpos, std::string ypos)
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
 * Resize the notification bubble to the desired width and height
 */
int AriaNotify::resize(void)
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
int AriaNotify::reposition(void)
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
void AriaNotify::cleanup(int sig)
{
    AriaSharedMem::remove();
    exit(sig);
}
