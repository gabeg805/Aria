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
#include <X11/Xlib.h>
#include <gtkmm.h>
#include <gdkmm.h>
#include <pangomm/fontdescription.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <string>

/* ***************************************
 * ***** DISPLAY NOTIFICATION BUBBLE *****
 * ***************************************
 */

AriaNotify::AriaNotify() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    bubble(Gtk::ORIENTATION_VERTICAL)
{
}

/* *******************************
 * ***** PRINT PROGRAM USAGE *****
 * *******************************
 */

void AriaNotify::usage(void)
{
    std::cout << "Usage: " << AriaAttribute::get("program") << " [option] <argument>\n"
              << "\n"
              << "Options:\n"
              << "    -h, --help                   Print program usage message\n"
              << "    -t, --title <text>           Title text\n"
              << "    -b, --body <text>            Body text\n"
              << "    -w, --width <size>           Width size\n"
              << "    -h, --height <size>          Height size\n"
              << "    -x, --xpos <position>        Location in the x-direction to move\n"
              << "    -y, --ypos <position>        Location in the y-direction to move\n"
              << "    --time <seconds>             Amount of time (in seconds) to display\n"
              << "    -o, --opacity <value>        Opacity (0 <= value <= 1)\n"
              << "    -m, --margin <value>         Box margins(0 < value)\n"
              << "    -f, --font <font>            Text font\n"
              << "    -ts, --title-size <size>     Size of text for the title\n"
              << "    -bs, --body-size <size>      Size of text for the body\n"
              << "    -bg, --background <color>    Color of the notification bubble\n"
              << "    -fg, --foreground <color>    Color of the text\n"
              << "\n"
              << "Arguments:\n"
              << "    <text>                       Text to display\n"
              << "    <size>                       Size of the notification bubble\n"
              << "    <position>                   X-Y position of the notitification bubble\n"
              << "    <value>                      An integer or double value\n"
              << "    <font>                       Text font\n"
              << "    <size>                       Text size\n"
              << "    <color>                      Hexadecimal color"
              << std::endl;
}

/* **********************************************
 * ***** INITIALIZE THE NOTIFICATION BUBBLE *****
 * **********************************************
 */

void AriaNotify::init(int argc, char** argv)
{
    AriaAttribute::set("program", *argv);
    std::string str;

    while ( *++argv != NULL ) {
        str = *argv++;

        if ( (str.compare("-h") == 0) || (str.compare("--help") == 0) ) {
            usage();
            exit(0);
        }
        else if ( (str.compare("-t") == 0) || (str.compare("--title") == 0) )
            AriaAttribute::set("title", *argv);
        else if ( (str.compare("-b") == 0) || (str.compare("--body") == 0) ) 
            AriaAttribute::set("body", *argv);
        else if ( (str.compare("-f") == 0) || (str.compare("--font") == 0) ) 
            AriaAttribute::set("font", *argv);
        else if ( (str.compare("-w") == 0) || (str.compare("--width") == 0) ) 
            AriaAttribute::set("width", *argv);
        else if ( (str.compare("-h") == 0) || (str.compare("--height") == 0) ) 
            AriaAttribute::set("height", *argv);
        else if ( (str.compare("-x") == 0) || (str.compare("--xpos") == 0) ) 
            AriaAttribute::set("xpos", *argv);
        else if ( (str.compare("-y") == 0) || (str.compare("--ypos") == 0) ) 
            AriaAttribute::set("ypos", *argv);
        else if ( str.compare("--time") == 0 )
            AriaAttribute::set("timer", *argv);
        else if ( (str.compare("-o") == 0) || (str.compare("--opacity") == 0) ) 
            AriaAttribute::set("opacity", *argv);
        else if ( (str.compare("-m") == 0) || (str.compare("--margin") == 0) ) 
            AriaAttribute::set("margin", *argv);
        else if ( (str.compare("-ts") == 0) || (str.compare("--title-size") == 0) ) 
            AriaAttribute::set("title_size", *argv);
        else if ( (str.compare("-bs") == 0) || (str.compare("--body-size") == 0) ) 
            AriaAttribute::set("body_size", *argv);
        else if ( (str.compare("-bg") == 0) || (str.compare("--background") == 0) ) 
            AriaAttribute::set("background", *argv);
        else if ( (str.compare("-fg") == 0) || (str.compare("--foreground") == 0) ) 
            AriaAttribute::set("foreground", *argv);
        else
            continue;
    }

    AriaAttribute::set_defaults();
}

/* ******************************************
 * ***** CREATE THE NOTIFICATION BUBBLE *****
 * ******************************************
 */

void AriaNotify::create(void)
{
    check_notify_text();
    set_title();
    set_size();
    set_background();
    set_foreground();
    set_margin();
    set_notify_title_text();
    set_notify_body_text();
    set_timer();
    this->add(bubble);
    this->show_all_children();
    set_position();
}

/* *****************************
 * ***** CHECK TEXT STATUS *****
 * *****************************
 */

void AriaNotify::check_notify_text(void)
{
    std::string title = AriaAttribute::get("title");
    std::string body  = AriaAttribute::get("body");
    std::string prog  = AriaAttribute::get("program");

    if ( title.empty() && body.empty() ) {
        std::cout << prog << ": No title or body text set." << std::endl;
        exit(1);
    }
}

/* **********************************
 * ***** SET NOTIFICATION TIMER *****
 * **********************************
 */

void AriaNotify::set_timer(void)
{
    int timer = atoi(AriaAttribute::get("timer").c_str());
    if ( timer <= 0 )
        return;

    Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, 
                                                         &AriaNotify::timeout), 
                                           timer);
}

bool AriaNotify::timeout()
{
    std::cout << "Timeout hiding" << std::endl;
    hide();
    return true;
}

/* *********************************
 * ***** SET NOTIFICATION TEXT *****
 * *********************************
 */

void AriaNotify::set_title(void)
{
    std::string prog = AriaAttribute::get("program");
    // this->set_title(prog);
}

void AriaNotify::set_notify_title_text(void)
{
    set_notify_text("title");
}

void AriaNotify::set_notify_body_text(void)
{
    set_notify_text("body");
}

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

/* ************************************
 * ***** SET NOTIFICATION SPACING *****
 * ************************************
 */

void AriaNotify::set_position(void)
{
    Display *dpy    = XOpenDisplay(NULL);
    int      screen = DisplayWidth(dpy, DefaultScreen(dpy));
    int      w      = 0;
    int      h      = 0;
    this->get_size((int&)w, (int&)h);

    int x = (screen - w) - atoi(AriaAttribute::get("xpos").c_str());
    int y = atoi(AriaAttribute::get("ypos").c_str());
    this->move(x, y);
}

void AriaNotify::set_size(void)
{
    int width  = atoi(AriaAttribute::get("width").c_str());
    int height = atoi(AriaAttribute::get("height").c_str());
    if ( width && height )
        this->set_default_size(width, height);
}

void AriaNotify::set_margin(void)
{
    int margin = atoi(AriaAttribute::get("margin").c_str());
    bubble.set_margin_top(margin);
    bubble.set_margin_bottom(margin);
    bubble.set_margin_start(margin);
    bubble.set_margin_end(margin);
}

/* **********************************
 * ***** SET NOTIFICATION COLOR *****
 * **********************************
 */

void AriaNotify::set_background(void)
{
    std::string color = AriaAttribute::get("background");
    if ( color.empty() )
        return;

    Gdk::RGBA background(color);
    this->override_background_color(background, Gtk::STATE_FLAG_NORMAL);
}


void AriaNotify::set_foreground(void)
{
    std::string color = AriaAttribute::get("foreground");
    if ( color.empty() )
        return;

    Gdk::RGBA foreground(color);
    this->override_color(foreground, Gtk::STATE_FLAG_NORMAL);
}
