// **********************************************************************************
// 
// Name:    NotificationBubble.cc
// Class:   <NotificationBubble>
// Author:  Gabriel Gonzalez
// Email:   gabeg@bu.edu
// License: The MIT License (MIT)
// 
// Description: The Aria notification bubble constructor and creator.
//              
// Notes: None.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/NotificationBubble.h"
#include "../hdr/Config.h"

#include <X11/Xlib.h>
#include <gtkmm.h>
#include <gdkmm.h>
#include <pangomm/fontdescription.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <iterator>
#include <string>



// ///////////////////////////////////////
// ///// DISPLAY NOTIFICATION BUBBLE /////
// ///////////////////////////////////////

NotificationBubble::NotificationBubble() :
    Gtk::Window(Gtk::WINDOW_POPUP),
    bubble(Gtk::ORIENTATION_VERTICAL)
{
    // Set default notification bubble variables
    def["title"]      = "Title";
    def["body"]       = "Body";
    def["font"]       = "GohuFont";
    def["width"]      = "50";
    def["height"]     = "50";
    def["xpos"]       = "0";
    def["ypos"]       = "0";
    def["timer"]      = "3";
    def["opacity"]    = "1";
    def["margin"]     = "0";
    def["title-size"] = "16";
    def["body-size"]  = "12";
    def["background"] = "#FFFFFF";
    def["foreground"] = "#000000";
    
    attr["program"]    = "";
    attr["title"]      = "";
    attr["body"]       = "";
    attr["font"]       = "";
    attr["width"]      = "";
    attr["height"]     = "";
    attr["xpos"]       = "";
    attr["ypos"]       = "";
    attr["timer"]      = "";
    attr["opacity"]    = "";
    attr["margin"]     = "";
    attr["title-size"] = "";
    attr["body-size"]  = "";
    attr["background"] = "";
    attr["foreground"] = "";
}



// ///////////////////////////////
// ///// PRINT PROGRAM USAGE /////
// ///////////////////////////////

// Print program usage
void NotificationBubble::usage() {
    std::cout << "Usage: " << attr["program"] << " [option] <argument>\n"
              << "\n"
              << "Options:" << std::endl
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



// //////////////////////////////////////////
// ///// CREATE THE NOTIFICATION BUBBLE /////
// //////////////////////////////////////////

// Initialize the notification bubble
void NotificationBubble::init(int argc, char** argv) {
    
    // Read command line inputs
    attr["program"] = argv[0];
    size_t i = 1;
    
    while ( argv[i] != 0 ) {
        std::string str = argv[i];
        
        if ( (str.compare("-h") == 0) || (str.compare("--help") == 0)) {
            usage();
            exit(0);
        } else if ( (str.compare("-t") == 0) || (str.compare("--title") == 0) ) 
            attr["title"] = argv[i+1];
        else if ( (str.compare("-b") == 0) || (str.compare("--body") == 0) ) 
            attr["body"] = argv[i+1];
        else if ( (str.compare("-f") == 0) || (str.compare("--font") == 0) ) 
            attr["font"] = argv[i+1];
        else if ( (str.compare("-w") == 0) || (str.compare("--width") == 0) ) 
            attr["width"] = argv[i+1];
        else if ( (str.compare("-h") == 0) || (str.compare("--height") == 0) ) 
            attr["height"] = argv[i+1];
        else if ( (str.compare("-x") == 0) || (str.compare("--xpos") == 0) ) 
            attr["xpos"] = argv[i+1];
        else if ( (str.compare("-y") == 0) || (str.compare("--ypos") == 0) ) 
            attr["ypos"] = argv[i+1];
        else if ( str.compare("--time") == 0 )
            attr["timer"] = argv[i+1];
        else if ( (str.compare("-o") == 0) || (str.compare("--opacity") == 0) ) 
            attr["opacity"] = argv[i+1]; 
        else if ( (str.compare("-m") == 0) || (str.compare("--margin") == 0) ) 
            attr["margin"] = argv[i+1]; 
        else if ( (str.compare("-ts") == 0) || (str.compare("--title-size") == 0) ) 
            attr["title_size"] = argv[i+1];
        else if ( (str.compare("-bs") == 0) || (str.compare("--body-size") == 0) ) 
            attr["body_size"] = argv[i+1];
        else if ( (str.compare("-bg") == 0) || (str.compare("--background") == 0) ) 
            attr["background"] = argv[i+1]; 
        else if ( (str.compare("-fg") == 0) || (str.compare("--foreground") == 0) ) 
            attr["foreground"] = argv[i+1]; 
        else
            ;
        
        ++i;
    }
    
    // Set the missing notification bubble attributes
    std::map<std::string, std::string>::iterator it;
    
    for ( it = attr.begin(); it != attr.end(); ++it) {
        std::string key   = it->first;
        std::string value = it->second;
        
        if ( value.compare("") == 0 ) {
            attr[key] = Config::fetch(key);
            
            bool isEmpty       = (attr[key].compare("") == 0);
            bool isTitleEmpty  = (key.compare("title") == 0);
            bool isBodyEmpty   = (key.compare("body") == 0);
            bool isWidthEmpty  = (key.compare("width") == 0);
            bool isHeightEmpty = (key.compare("height") == 0);
            
            if ( isEmpty && !isTitleEmpty && !isBodyEmpty && !isWidthEmpty && !isHeightEmpty )
                attr[key] = def[key];
        } 
    }
}



// Create the notification bubble
void NotificationBubble::create() {
    
    // Check title and body
    bool isTitleEmpty = (attr["title"].compare("") == 0);
    bool isBodyEmpty  = (attr["body"].compare("") == 0);
    
    if ( isTitleEmpty && isBodyEmpty ) {
        std::cout << attr["program"] << ": No title or body specified.\n" << std::endl;
        usage();
        exit(1);
    }
    
    // Coloring
    Gdk::RGBA back(attr["background"]);
    Gdk::RGBA fore(attr["foreground"]);
    
    // Define variables to be used in setup
    int width         = atoi( attr["width"].c_str() );
    int height        = atoi( attr["height"].c_str() );
    int timer         = atoi( attr["timer"].c_str() );
    int margin        = atoi( attr["margin"].c_str() );
    int title_size    = atoi( attr["title-size"].c_str() ) * PANGO_SCALE;
    int body_size     = atoi( attr["body-size"].c_str() ) * PANGO_SCALE;
    
    // Setup the notification bubble
    this->set_title("Aria");
    if ( width && height ) {
        this->set_default_size( width, height );
    }
    this->override_background_color(back, Gtk::STATE_FLAG_NORMAL);
    this->override_color(fore, Gtk::STATE_FLAG_NORMAL);
    
    // Setup the notification bubble container
    bubble.set_margin_top( margin );
    bubble.set_margin_bottom( margin );
    bubble.set_margin_start( margin );
    bubble.set_margin_end( margin );
    
    // Setup the title label
    if ( !isTitleEmpty ) {
        title_fd.set_family( attr["font"] );
        title_fd.set_size( title_size );
        title.set_text( attr["title"] );
        title.override_font( title_fd );
        title.override_color( fore );
        title.set_margin_top( margin/2 );
        title.set_margin_bottom( margin/2 );
        bubble.pack_start(title, Gtk::PACK_SHRINK);
    }
    
    // Setup the body label
    if ( !isBodyEmpty ) {
        body_fd.set_family( attr["font"] );
        body_fd.set_size( body_size );
        body.set_text( attr["body"] );
        body.override_font(body_fd);
        body.override_color(fore);
        body.set_margin_top( margin/2 );
        body.set_margin_bottom( margin/2 );
        
        if ( isTitleEmpty )
            bubble.pack_start(body, Gtk::PACK_SHRINK);
        else
            bubble.pack_end(body, Gtk::PACK_SHRINK);
    }
    
    // Add the containers to the notification bubble
    this->add(bubble);
    
    // Add the timeout for the notification bubble
    Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, 
                                                         &NotificationBubble::timeout), 
                                           timer);
    
    // Set size of the notification bubble
    Display *dpy = XOpenDisplay(NULL);
    int screen = DisplayWidth( dpy, DefaultScreen(dpy) );
    int w = 0;
    int h = 0;
    this->get_size((int&)w, (int&)h);
    
    int xpos          = (screen - w) - atoi( attr["xpos"].c_str() );
    int ypos          = atoi( attr["ypos"].c_str() );
    
    this->move( xpos, ypos );
}



// Notification bubble timeout
bool NotificationBubble::timeout() {
    hide();
    return true;
}
