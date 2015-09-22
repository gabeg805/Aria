/* *****************************************************************************
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
 * *****************************************************************************
 */

/* Includes */
#include "AriaNotify.h"
#include "AriaAttribute.h"
#include "AriaMap.h"
#include "AriaUtility.h"
#include <gtkmm.h>
#include <gdkmm.h>
#include <pangomm/fontdescription.h>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <string>

// static void cleanup(int sig);

/* ************************************************************************** */
/**
 * Construct the notification window and container.
 */
AriaNotify::AriaNotify() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    bubble(Gtk::ORIENTATION_VERTICAL)
{
}

/* ************************************************************************** */
/**
 * @brief Initialize the notification bubble.
 * 
 * @details Check inputs and set up signal handlers for cleanup.
 * 
 * @param argv the command line argument vector.
 */
void AriaNotify::init(char **argv)
{
    int status = AriaAttribute::init(argv);
    if ( status != 0 ) {
        AriaUtility::usage();
        exit(0);
    }

    std::string title = AriaAttribute::getstr("title");
    std::string body  = AriaAttribute::getstr("body");
    if ( title.empty() && body.empty() )
        AriaUtility::error("No title or body text set");

    // std::signal(SIGHUP,  cleanup);
    std::signal(SIGINT,  cleanup);
    std::signal(SIGQUIT, cleanup);
    // std::signal(SIGKILL, cleanup);
    // std::signal(SIGSEGV, cleanup);
    std::signal(SIGTERM, cleanup);
}

/* ************************************************************************** */
/**
 * @brief Display the notification bubble.
 */
void AriaNotify::show(void)
{
    this->add(bubble);
    this->show_all_children();
    set_size();
}

/* ************************************************************************** */
/**
 * @brief Move the notification bubble.
 * 
 * @details Determine the appropriate location to move the notification
 *          bubble such that there will be no overlap with other
 *          notifications. Stores this position in a shared memory region as
 *          a means of interprocess communication with other Aria
 *          applications.
 */
void AriaNotify::position(void)
{
    struct MapData data = {.id = getpid(),
                           .x  = AriaAttribute::getint("xpos"),
                           .y  = AriaAttribute::getint("ypos"),
                           .w  = AriaAttribute::getint("width"),
                           .h  = AriaAttribute::getint("height")};
    AriaMap::store(&data, 10);

    data.x = (AriaAttribute::getint("screen") - data.w) - data.x;
    this->move(data.x, data.y);
}

/* ************************************************************************** */
/**
 * @brief Cleanup any memory mapped data.
 * 
 * @details Shuts down the program after cleaning up stored data. Used as a
 *          static member function due to its status as a signal handler.
 * 
 * @params sig the signal that was raised. Incidentally, also the exit status.
 */
void AriaNotify::cleanup(int sig)
{
    AriaMap::cleanup();
    exit(sig);
}

/* ************************************************************************** */
/**
 * @brief Set the notification bubble title text.
 */
void AriaNotify::set_title(void)
{
    set_notify_text("title");
}

/* ************************************************************************** */
/**
 * @brief Set the notification bubble body text.
 */
void AriaNotify::set_body(void)
{
    set_notify_text("body");
}

/* ************************************************************************** */
/**
 * @brief General notification text setter.
 * 
 * @details Set the font family, font size, and text for the given
 *          notification field.
 * 
 * @param field the notification field to setup - either title or body.
 */
void AriaNotify::set_notify_text(std::string field)
{
    if ( !AriaAttribute::getstr(field).empty() ) {
        Gtk::Label *label     = Gtk::manage(new Gtk::Label());
        std::string font      = AriaAttribute::getstr("font");
        std::string font_size = AriaAttribute::getstr(field + "-size");
        std::string text      = AriaAttribute::getstr(field);
        Pango::FontDescription fd;

        if ( !font.empty() )
            fd.set_family(font);
        if ( !font_size.empty() )
            fd.set_size(atoi(font_size.c_str()) * PANGO_SCALE);
        if ( !text.empty() )
            label->set_text(text);

        label->override_font(fd);
        bubble.pack_start(*label, Gtk::PACK_SHRINK);
    }
}

/* ************************************************************************** */
/**
 * @brief Set the background color of the notification bubble.
 */
void AriaNotify::set_background(void)
{
    std::string color = AriaAttribute::getstr("background");
    if ( color.empty() )
        return;

    Gdk::RGBA background(color);
    this->override_background_color(background, Gtk::STATE_FLAG_NORMAL);
}

/* ************************************************************************** */
/**
 * @brief Set the foreground (text) color of the notification bubble.
 */
void AriaNotify::set_foreground(void)
{
    std::string color = AriaAttribute::getstr("foreground");
    if ( color.empty() )
        return;

    Gdk::RGBA foreground(color);
    this->override_color(foreground, Gtk::STATE_FLAG_NORMAL);
}

/* ************************************************************************** */
/**
 * @brief Set the margin of the notification bubble text.
 */
void AriaNotify::set_margin(void)
{
    int margin = atoi(AriaAttribute::getstr("margin").c_str());
    bubble.set_margin_top(margin);
    bubble.set_margin_bottom(margin);
    bubble.set_margin_start(margin);
    bubble.set_margin_end(margin);
}

/* ************************************************************************** */
/**
 * @brief Set the notification bubble size.
 */
void AriaNotify::set_size(void)
{
    /* Width */
    std::string width_str  = AriaAttribute::getstr("width");
    int         width;
    int         tmp;
    if ( width_str.compare("0") == 0 )
        this->get_size(width, tmp);
    else
        width = atoi(width_str.c_str());

    /* Height */
    std::string height_str = AriaAttribute::getstr("height");
    int         height;
    if ( height_str.compare("0") == 0 )
        this->get_size(tmp, height);
    else
        height = atoi(height_str.c_str());

    /* Size */
    this->set_default_size(width, height);
    AriaAttribute::setint("width",  width);
    AriaAttribute::setint("height", height);
}

/* ************************************************************************** */
/**
 * @brief Set the notification bubble timer.
 * 
 * @details Closes the notification bubble after the timeout.
 */
void AriaNotify::set_timer(void)
{
    int timer = atoi(AriaAttribute::getstr("timer").c_str());
    if ( timer <= 0 )
        return;

    Glib::signal_timeout().connect_seconds_once(
        sigc::bind<int>(sigc::ptr_fun(&cleanup), 0), timer);
}
