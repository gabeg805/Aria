/* *****************************************************************************
 * 
 * Name:    aria.cc
 * Class:   main()
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: $ g++ -g -Wall `pkg-config gtkmm-3.0 --cflags --libs` -o aria *.cc
 *         $ ./aria
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

/* **********************************
 * ***** DISPLAY ARIA STATUSBAR *****
 * **********************************
 */

int main(int argc, char** argv) {
    /* To allow multiple windows, change create text to "" */
    Glib::RefPtr<Gtk::Application> app =
        Gtk::Application::create("aria.notify.bubble");

    AriaNotify Aria;
    Aria.init(argc, argv);
    Aria.create();

    return app->run(Aria);
}
