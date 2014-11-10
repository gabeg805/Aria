// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     noti.c
// 
// 
// SYNTAX: 
// 
//     ./noti -t [TITLE] -b [BODY] --time [TIME]
//            -x [XPOS] -y [YPOS] -o [OPACITY] -m [MARGIN]
//            --font [FONT] -ts [TITLE_SIZE] -bs [BODY_SIZE]
//            -fg [FG_COLOR] -bg [BG_COLOR]
// 
// 
// PURPOSE:
// 
//     A custom notification bubble I made which allows the user to control such 
//     things as: font, text size, text color, background color, and background 
//     transparency.
// 
// 
// KEYWORDS:
// 
//     -t     : Notification bubble title
//     -b     : Notification bubble body
//     --time : Notification bubble timeout
//     -x     : X position 
//     -y     : Y position 
//     -o     : Notification bubble opacity  
//     -m     : Text margin
//     --font : Text font
//     -ts    : Title text size
//     -bs    : Body text size
//     -fg    : Foreground text color
//     -bg    : Backgroun notification bubble color
// 
// 
// FUNCTIONS:
// 
//     main : Display the notification bubble
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Display Notification Bubble
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 28 2014 <> created
// 
//     gabeg Oct 28 2014 <> Rounded the edges of the notification bubble.
// 
//     gabeg Nov 09 2014 <> Broke up the program into two files, one with the main
//                          function an the other is the library of functions that
//                          are used. Added a "-h" parameter that prints the usage of
//                          the program.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/noti.h"
#include "../hdr/notilib.h"

#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Declares
int WIDTH;
int HEIGHT;
char *TITLE_TEXT;
char *BODY_TEXT;

int XPOS         = 30;
int YPOS         = 30;
int MARGIN       = 10;
int TIMER        = 3000;
double OPACITY   = 0.75;
char *FG_COLOR   = "#000000";
char *BG_COLOR   = "#EEB111";
char *FONT       = "DejaVu Sans";
int TITLE_SIZE   = 18*1024;
int BODY_SIZE    = 12*1024;

int SCALE     =    1024;
int PSCALE    =    65535 / 255;
double CSCALE =    1.0 / 255.0;



// ///////////////////////////////////////
// ///// DISPLAY NOTIFICATION BUBBLE /////
// ///////////////////////////////////////

// Run notification bubble 
int main(int argc, char *argv[]) {
    
    // Initialize gtk toolkit
    gtk_init(&argc, &argv);
    
    GtkWidget *window    = gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget *frame     = gtk_drawing_area_new();
    GtkWidget *frame_box = gtk_overlay_new();
    GtkWidget *text_box  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    // Define global variables
    if ( setup_parameters(argv) )
        return 1;
    
    // Setup notification window and get window size
    if ( setup_noti_text_box(text_box) )
        return 1;
    setup_noti_win(window, frame, text_box);
    setup_noti_win_dims(window);
    
    // Old text box was just used to get window size, remove it
    gtk_container_remove(GTK_CONTAINER(window), text_box); 
    
    // Redefine text box now that window size is known, setup title and body 
    text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    setup_noti_text_box(text_box);
    
    // Add everything to the main window
    gtk_container_add(GTK_CONTAINER(frame_box), frame);
    gtk_overlay_add_overlay(GTK_OVERLAY(frame_box), text_box);
    gtk_container_add(GTK_CONTAINER(window), frame_box);
    
    // Display the label
    gtk_widget_show_all(window);
    
    // Run the gtk loop
    gtk_main();
    
    return 0;
}
