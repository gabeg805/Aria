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
#include <X11/Xlib.h>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <string>

static void cleanup(int sig);

/* ***************************************
 * ***** DISPLAY NOTIFICATION BUBBLE *****
 * ***************************************
 */

AriaNotify::AriaNotify() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    bubble(Gtk::ORIENTATION_VERTICAL)
{
}

/* ******************************************
 * ***** CREATE THE NOTIFICATION BUBBLE *****
 * ******************************************
 */

/* Initialize notification */
void AriaNotify::init(char **argv)
{
    int status = AriaAttribute::init(argv);
    if ( status != 0 ) {
        AriaUtility::usage();
        exit(0);
    }

    std::string title = AriaAttribute::get("title");
    std::string body  = AriaAttribute::get("body");
    if ( title.empty() && body.empty() )
        AriaUtility::error("No title or body text set");

    std::signal(SIGHUP,  cleanup);
    std::signal(SIGINT,  cleanup);
    std::signal(SIGQUIT, cleanup);
    std::signal(SIGKILL, cleanup);
    std::signal(SIGSEGV, cleanup);
    std::signal(SIGTERM, cleanup);
}

/* Display notification */
void AriaNotify::show(void)
{
    this->add(bubble);
    this->show_all_children();
    set_size();
}

/* Move notification to position and save coordinates */
void AriaNotify::position(void)
{
    struct MapData data = {.id = getpid(),
                           .x  = get_xpos(),
                           .y  = get_ypos(),
                           .w  = get_width(),
                           .h  = get_height()};
    AriaMap::store(&data, 10);

    data.x = (get_screen_size() - data.w) - data.x;
    this->move(data.x, data.y);
}

/* Exit action when notification times out */
bool AriaNotify::timeout()
{
    AriaMap::cleanup();
    hide();
    return true;
}

/* ***************************************
 * ***** SET NOTIFICATION ATTRIBUTES *****
 * ***************************************
 */

/* Set title text */
void AriaNotify::set_title(void)
{
    set_notify_text("title");
}

/* Set body text */
void AriaNotify::set_body(void)
{
    set_notify_text("body");
}

/* General text setter */
void AriaNotify::set_notify_text(std::string key)
{
    if ( !AriaAttribute::get(key).empty() ) {
        Gtk::Label *label     = Gtk::manage(new Gtk::Label());
        std::string font      = AriaAttribute::get("font");
        std::string font_size = AriaAttribute::get(key + "-size");
        std::string text      = AriaAttribute::get(key);
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

/* Set background color */
void AriaNotify::set_background(void)
{
    std::string color = AriaAttribute::get("background");
    if ( color.empty() )
        return;

    Gdk::RGBA background(color);
    this->override_background_color(background, Gtk::STATE_FLAG_NORMAL);
}

/* Set foreground color */
void AriaNotify::set_foreground(void)
{
    std::string color = AriaAttribute::get("foreground");
    if ( color.empty() )
        return;

    Gdk::RGBA foreground(color);
    this->override_color(foreground, Gtk::STATE_FLAG_NORMAL);
}

/* Set text margins */
void AriaNotify::set_margin(void)
{
    int margin = atoi(AriaAttribute::get("margin").c_str());
    bubble.set_margin_top(margin);
    bubble.set_margin_bottom(margin);
    bubble.set_margin_start(margin);
    bubble.set_margin_end(margin);
}

/* Set bubble size */
void AriaNotify::set_size(void)
{
    int width  = get_width();
    int height = get_height();
    if ( width && height )
        this->set_default_size(width, height);
}

/* Set display timer */
void AriaNotify::set_timer(void)
{
    int timer = atoi(AriaAttribute::get("timer").c_str());
    if ( timer <= 0 )
        return;

    Glib::signal_timeout().connect_seconds(
        sigc::mem_fun(*this, &AriaNotify::timeout), timer);
}

/* ***************************************
 * ***** GET NOTIFICATION DIMENSIONS *****
 * ***************************************
 */

void cleanup(int sig)
{
    AriaMap::cleanup();
    exit(sig);
}

/* Spatial positioning */
int AriaNotify::get_xpos(void)
{
    static int xpos = atoi(AriaAttribute::get("xpos").c_str());
    return xpos;
}

int AriaNotify::get_ypos(void)
{
    static int ypos = atoi(AriaAttribute::get("ypos").c_str());
    return ypos;
}

/* Dimensions */
int AriaNotify::get_width(void)
{
    static int width  = atoi(AriaAttribute::get("width").c_str());
    static int height = 0;
    if ( width > 0 )
        return width;
    else
        this->get_size(width, height);
    return width;
}

int AriaNotify::get_height(void)
{
    static int width  = 0;
    static int height = atoi(AriaAttribute::get("height").c_str());
    if ( height > 0 )
        return height;
    else
        this->get_size(width, height);
    return height;
}

int AriaNotify::get_screen_size(void)
{
    static Display *dpy    = XOpenDisplay(NULL);
    static int      screen = DisplayWidth(dpy, DefaultScreen(dpy));
    return screen;
}
