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

/* *****************************************************************************
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
 * *****************************************************************************
 */

/* Includes */
#include "AriaNotify.h"
#include <gtkmm.h>

/* Create and display the Aria statusbar */
int main(int argc, char** argv)
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("");

    AriaNotify Aria;
    Aria.init(argv);
    Aria.show();
    Aria.movepos();

    return app->run(Aria);
}
