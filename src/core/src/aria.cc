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
