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

int main(int argc, char** argv)
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("");

    AriaNotify Aria;
    Aria.init(argv);
    Aria.set_title();
    Aria.set_body();
    Aria.set_background();
    Aria.set_foreground();
    Aria.set_margin();
    Aria.set_timer();
    Aria.show();
    Aria.position();

    return app->run(Aria);
}
