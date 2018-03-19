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
#include "AriaNotify.h"
#include "commandline.h"
#include <gtkmm.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

/* Create and display the Aria statusbar */
int main(int argc, char** argv)
{
    /* Command line options */
    const commandline::options opts{
        commandline::option{"-h", "--help",        "Print program usage."},
        commandline::option{"-t", "--title",       "Title.", "TITLE"},
        commandline::option{"-b", "--body",        "Body text.", "BODY"},
        commandline::option{"-X", "--xpos",        "X-position on screen.", "POS"},
        commandline::option{"-Y", "--ypos",        "Y-position on screen.", "POS"},
        commandline::option{"-m", "--margin",      "Margin all around the notification.", "MARGIN"},
        commandline::option{"-W", "--width",       "Width.", "WIDTH"},
        commandline::option{"-H", "--height",      "Height.", "HEIGHT"},
        commandline::option{"-o", "--opacity",     "Opacity (0 <= opacity <= 1).", "OPACITY"},
        commandline::option{"-T", "--time",        "Amount of time to display notification.", "TIME"},
        commandline::option{"-f", "--font",        "Font of text to display.", "FONT"},
        commandline::option{"-ts", "--title-size", "Size of title text.", "SIZE"},
        commandline::option{"-bs", "--body-size",  "Size of body text.", "SIZE"},
        commandline::option{"-bg", "--background", "Background color.", "COLOR"},
        commandline::option{"-fg", "--foreground", "Foreground color.", "COLOR"}
    };

    /* Process command line arguments */
    commandline::parser::make_type cli{commandline::parser::init(std::move(opts))};
    std::vector<std::string> args{argv + 1, argv + argc};
    cli->process_input(args);
    cli->process_config();
    /* Add a check for help */
    /* Long options not working */

    /* Build notification bubble */
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("");
    AriaNotify Aria;

    Aria.build(cli->get_values());
    Aria.show();

    return app->run(Aria);
}
