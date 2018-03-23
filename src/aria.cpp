/*!
 * \mainpage Aria Notification Bubble
 * 
 * \section intro_sec Introduction
 * 
 * A gtkmm based C++ notification bubble that is designed to be highly
 * configurable, with the user able to customize the location it is displayed,
 * font, text size, background color, foreground color, and more.
 * 
 * \section install_sec Installation
 * 
 * Step 1: Run '$ make' to build the program.
 * 
 * Step 2: Run '$ make test' to test out basic functionality.
 * 
 * Step 3: Run '$ ./aria --help' for more information on the program.
 */

/* -----------------------------------------------------------------------------
 *  
 * Name:    aria.cc
 * Class:   main()
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: aria [option] [--title <title>] [--body <body>]
 * 
 * Description: Creates the Aria Notification Bubble.
 * 
 * Notes: None.
 * 
 * -----------------------------------------------------------------------------
 */

/* Includes */
#include "arianotification.hpp"
#include "ariacommandline.hpp"
#include <gtkmm.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace aria;

/* Create and display the Aria statusbar */
int main(int argc, char** argv)
{
    /* Command line options */
    commandline::optlist_t options{
        {"-h", "--help",        "",        commandline::no_argument,       "Print program usage."},
        {"-t", "--title",       "title",   commandline::required_argument, "Title of the notification."},
        {"-b", "--body",        "body",    commandline::required_argument, "Body of the notification."},
        {"-X", "--xpos",        "pos",     commandline::required_argument, "X-coordinate of where to put the notification on the screen."},
        {"-Y", "--ypos",        "pos",     commandline::required_argument, "Y-coordinate of where to put the notification on the screen."},
        {"-m", "--margin",      "margin",  commandline::required_argument, "Margin all around the notification."},
        {"-W", "--width",       "width",   commandline::required_argument, "Width of the notification."},
        {"-H", "--height",      "height",  commandline::required_argument, "Height of the notification."},
        {"-o", "--opacity",     "opacity", commandline::required_argument, "Opacity of the notification. [Default: 0.5]"},
        {"-T", "--time",        "time",    commandline::required_argument, "Amount of time to display the notification. [Default: 2s]"},
        {"-f", "--font",        "font",    commandline::required_argument, "Font to display text in. [Default: Dejavu Sans]"},
        {"-ts", "--title-size", "size",    commandline::required_argument, "Size of title text. [Default: 16]"},
        {"-bs", "--body-size",  "size",    commandline::required_argument, "Size of body text. [Default: 12]"},
        {"-bg", "--background", "color",   commandline::required_argument, "Background color. [Default: 0xffa5d0]"},
        {"-fg", "--foreground", "color",   commandline::required_argument, "Foreground color. [Default: 0xffffff]"}
    };

    /* Process command line arguments */
    // commandline::interface* cli = new commandline::interface(options);
    commandline::interface cli(options);
    cli.parse(argv);
    // cli.print();

    /* Build notification bubble */
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("");
    aria::notification Aria;

    Aria.build(cli);
    // Aria.show();

    return app->run(Aria);
    // return 0;
}
