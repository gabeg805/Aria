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
#include <string>

/**
 * Construct the notification window and container.
 */
AriaNotify::AriaNotify() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    bubble(Gtk::ORIENTATION_VERTICAL)
{
    this->m_width = "";
    this->m_height = "";
    this->m_xpos = "";
    this->m_ypos = "";

    std::signal(SIGINT,  cleanup);
    std::signal(SIGQUIT, cleanup);
    std::signal(SIGTERM, cleanup);
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
    if (this->set_time(time) < 0) {
        return 5;
    }
    if (this->set_size(width, height) < 0) {
        return 6;
    }
    if (this->set_position(xpos, ypos) < 0) {
        return 7;
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
    label->set_text(text);
    label->override_font(fd);
    bubble.pack_start(*label, Gtk::PACK_SHRINK);
    return 0;
}

/**
 * Set the background color
 */
int AriaNotify::set_background(std::string color)
{
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
    this->m_width = width;
    this->m_height = height;
    return 0;
}

/**
 * Set the notification bubble position.
 */
int AriaNotify::set_position(std::string xpos, std::string ypos)
{
    this->m_xpos = std::move(xpos);
    this->m_ypos = std::move(ypos);
    return 0;
}

/**
 * Resize the notification bubble to the desired width and height
 */
int AriaNotify::resize(void)
{
    int w = std::stoi(m_width);
    int h = std::stoi(m_height);
    int tmp;

    if (m_width.compare("0") == 0) {
        this->get_size(w, tmp);
    }
    if (m_height.compare("0") == 0) {
        this->get_size(tmp, h);
    }

    this->set_default_size(w, h);
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
                                 .x    = std::stoi(m_xpos),
                                 .y    = std::stoi(m_ypos),
                                 .w    = std::stoi(m_width),
                                 .h    = std::stoi(m_height)};
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
