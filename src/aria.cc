// **********************************************************************************
// 
// Name:    aria.cc
// Class:   main()
// Author:  Gabriel Gonzalez
// Email:   gabeg@bu.edu
// License: The MIT License (MIT)
// 
// Syntax: $ g++ -g -Wall `pkg-config gtkmm-3.0 --cflags --libs` src/*.cc -o aria
//         $ ./aria
// 
// Description: Creates the Aria notification bubble unit.
//              
// Notes: None.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/NotificationBubble.h"

#include <gtkmm.h>



// ///////////////////////////////////
// ///// DISPLAY ARIA STATUSBAR /////
// ///////////////////////////////////

// Display the aria statusbar
int main(int argc, char** argv) {
    
    // Initialize gtkmm application
    Glib::RefPtr<Gtk::Application> app =
        Gtk::Application::create("aria.notify.bubble");
    
    // Setup notification bubble
    NotificationBubble  Aria;
    Aria.init(argc, argv);
    Aria.create();
    
    Aria.show_all_children();
    
    return app->run(Aria);
}
