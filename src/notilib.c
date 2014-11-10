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
//     N/A
// 
// 
// PURPOSE:
// 
//     Library of functions for the custom notification bubble I made that allows the
//     user to control such things as: font, text size, text color, background color,
//     and background transparency.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     setup_parameters    : Read program input parameters
// 
//     enable_transparency : Enable widget transparency
// 
//     draw_frame          : Draw the notification bubble frame
//     draw_window         : Draw the notification bubble window
//     quit                : Close the notification bubble
// 
//     strtohex            : Convert string to hex
//     get_color           : Convert to rgb from hex
//     setup_noti_win      : Setup window attributes
//     setup_noti_win_dims : Setup notification window dimensions
// 
//     setup_noti_title    : Setup title text attributes
//     setup_noti_body     : Setup body text attributes
//     setup_noti_text_box : Setup notification text box
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Read Input Parameters
//     * Enable Widget Transparency
//     * Draw Notification Bubble
//     * Get Color of Notification Bubble
//     * Setup Notification Bubble Window
//     * Setup Title and Body Attributes
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Nov 09 2014 <> Created.
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
int setup_parameters(char *argv[]);
void enable_transparency(GtkWidget *widget);
void draw_frame(cairo_t *cr);
gboolean draw_window(GtkWidget *widget);
gboolean quit(GtkWindow *window);
int strtohex(char *str);
int get_color(char *color, char letter);
void setup_noti_win(GtkWidget *window, GtkWidget *frame, GtkWidget *box);
void setup_noti_win_dims(GtkWidget *window);
int setup_noti_title(GtkWidget *label);
int setup_noti_body(GtkWidget *label);
int setup_noti_text_box(GtkWidget *box);



// /////////////////////////////////
// ///// READ INPUT PARAMETERS /////
// /////////////////////////////////

// Read program input parameters
int setup_parameters(char *argv[]) {
    
    // Determine which parameters were defined
    int i = 0;
    while ( argv[i+1] != NULL ) {
        ++i;
        
        if ( !strcmp("-h", argv[i]) ) {
            printf("%s \n\t %s \n\t %s \n\t %s \n",
                   "Usage: noti -t [TITLE] -b [BODY] --time [TIME]",
                   "-x [XPOS] -y [YPOS] -o [OPACITY] -m [MARGIN]",
                   "--font [FONT] -ts [TITLE_SIZE] -bs [BODY_SIZE]",
                   "-fg [FG_COLOR] -bg [BG_COLOR]");
            return 1;
        } else if ( !strcmp("-t", argv[i]) ) 
            TITLE_TEXT = argv[i+1];
         else if ( !strcmp("-b", argv[i]) ) 
             BODY_TEXT = argv[i+1];
         else if ( !strcmp("-x", argv[i]) ) 
             XPOS = atoi(argv[i+1]);
         else if ( !strcmp("-y", argv[i]) ) 
             YPOS = atoi(argv[i+1]);
         else if ( !strcmp("-o", argv[i]) ) 
             OPACITY = atof(argv[i+1]); 
         else if ( !strcmp("-m", argv[i]) ) 
             MARGIN = atoi(argv[i+1]); 
         else if ( !strcmp("--time", argv[i]) ) 
             TIMER = atoi(argv[i+1])*1000;
         else if ( !strcmp("--font", argv[i]) ) 
             FONT = argv[i+1];
         else if ( !strcmp("--tsize", argv[i]) ) 
             TITLE_SIZE = atoi(argv[i+1]) * SCALE;
         else if ( !strcmp("--bsize", argv[i]) ) 
             BODY_SIZE = atoi(argv[i+1]) * SCALE;
         else if ( !strcmp("-fg", argv[i]) ) 
             FG_COLOR = argv[i+1]; 
         else if ( !strcmp("-bg", argv[i]) ) 
             BG_COLOR = argv[i+1]; 
         else 
             continue;
        
        ++i;
    }

    return 0;
}



// //////////////////////////////////////
// ///// ENABLE WIDGET TRANSPARENCY /////
// //////////////////////////////////////

// Enable widget transparency
void enable_transparency(GtkWidget *widget) {
    
    // To check if the display supports alpha channels, get the visual 
    GdkScreen *screen = gtk_widget_get_screen(widget);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    
    // Set widget visual
    gtk_widget_set_visual(widget, visual);
}



// ////////////////////////////////////
// ///// DRAW NOTIFICATION BUBBLE /////
// ////////////////////////////////////

// Draw the notification bubble frame
void draw_frame(cairo_t *cr) { 
    
    // Custom shape that could be wrapped in a function 
    double radius = HEIGHT / 5.0;
    double degrees = M_PI / 180.0;
    
    // Create the rouded rectangle
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr,   WIDTH-radius,   radius,          radius,   -90*degrees,     0*degrees);
    cairo_arc(cr,   WIDTH-radius,   HEIGHT-radius,   radius,     0*degrees,    90*degrees);
    cairo_arc(cr,   radius,         HEIGHT-radius,   radius,    90*degrees,   180*degrees);
    cairo_arc(cr,   radius,         radius,          radius,   180*degrees,   270*degrees);
    cairo_close_path (cr);
    
    // Get color and opacity to set window
    double r = get_color(BG_COLOR, 'r') * CSCALE;
    double g = get_color(BG_COLOR, 'g') * CSCALE;
    double b = get_color(BG_COLOR, 'b') * CSCALE;
    
    // Fill login frame
    cairo_set_source_rgba(cr, r, g, b, OPACITY); 
    cairo_fill_preserve(cr);
    cairo_stroke (cr);
}



// Draw the root window 
gboolean draw_window(GtkWidget *widget) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
    
    // Draw the window background
    cairo_set_source_rgba(cr, 0, 0, 0, 0);    
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    // Draw the login frame
    draw_frame(cr);
    cairo_destroy(cr);
    
    return FALSE;
}



// Close the notification bubble
gboolean quit(GtkWindow *window) {
    gtk_main_quit();
    return TRUE;
}



// ////////////////////////////////////////////
// ///// GET COLOR OF NOTIFICATION BUBBLE /////
// ////////////////////////////////////////////

// Convert string to hex
int strtohex(char *str) {
    int len = strlen(str);
    int i, start;
    int val = 0x0;
    
    // Check if hashtag is included as first character
    if ( str[0] == '#' )
        start = 1;
    else
        start = 0;
    
    // Loop through all characters in string
    for (i = start; i < len; i++ ) {
        
        // Amount to shift character over for "val"
        int shift = (len-i-1) << 2;
        int sub;
        
        // Check if character is lower/upper case or a number
        if ( str[i] >= 'a' && str[i] <= 'f' )
            sub = 0x57;
        else if ( str[i] >= 'A' && str[i] <= 'F' ) 
            sub = 0x37;
        else if ( str[i] >= '0' && str[i] <= '9' )
            sub = 0x30;
        else
            return 0;
        
        // Store color hex value 
        val = val + ( (str[i] - sub) << shift );
    }
    
    return val;
}



// Convert to rgb from hex
int get_color(char *color, char letter) {
    
    int val = 0x0;
    
    if ( letter == 'r' ) 
        val = strtohex(color) >> 16;
    if ( letter == 'g' ) 
        val = strtohex(color) >> 8;
    if ( letter == 'b' ) 
        val = strtohex(color);
    
    return (val & 0xFF);
}



// ////////////////////////////////////////////
// ///// SETUP NOTIFICATION BUBBLE WINDOW /////
// ////////////////////////////////////////////

// Setup window attributes
void setup_noti_win(GtkWidget *window, GtkWidget *frame, GtkWidget *box) {

    // Set window attributes
    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_NORTH_EAST);
    
    // Enable window transparency
    enable_transparency(window);
    
    // GTK signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(frame), "draw", G_CALLBACK(draw_window), NULL);
    g_timeout_add(TIMER, (GSourceFunc)quit, window);         
}



// Setup notification window dimensions
void setup_noti_win_dims(GtkWidget *window) {
    
    // Display all widgets so window size is set
    gtk_widget_show_all(window);
    
    // Get X11 window attributes
    Display *display = XOpenDisplay(NULL);
    Screen *screen = DefaultScreenOfDisplay(display);
    
    // Get widget size
    gtk_window_get_size(GTK_WINDOW(window), &WIDTH, &HEIGHT);
    
    // Resize and move widget
    gtk_window_resize(GTK_WINDOW(window), WIDTH, HEIGHT);
    gtk_window_move(GTK_WINDOW(window), screen->width-WIDTH-XPOS, YPOS);
}



// ///////////////////////////////////////////
// ///// SETUP TITLE AND BODY ATTRIBUTES /////
// ///////////////////////////////////////////

// Setup title text attributes
int setup_noti_title(GtkWidget *label) {
    
    // Make sure title text was set
    if ( TITLE_TEXT == NULL ) 
        return 1;
    
    // Get color to set text
    int r = get_color(FG_COLOR, 'r') * PSCALE;
    int g = get_color(FG_COLOR, 'g') * PSCALE;
    int b = get_color(FG_COLOR, 'b') * PSCALE;
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new((const char*)FONT);
    PangoAttribute *attrSize = pango_attr_size_new(TITLE_SIZE);
    PangoAttribute *attrColor = pango_attr_foreground_new(r, g, b);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrColor);
    
    // Define title text string
    gtk_label_set_text(GTK_LABEL(label), TITLE_TEXT);
    gtk_label_set_attributes(GTK_LABEL(label), attrList);

    return 0;
}



// Setup body text attributes
int setup_noti_body(GtkWidget *label) {
    
    // Make sure title text was set
    if ( BODY_TEXT == NULL ) 
        return 1;
    
    // Get color to set text
    int r = get_color(FG_COLOR, 'r') * PSCALE;
    int g = get_color(FG_COLOR, 'g') * PSCALE;
    int b = get_color(FG_COLOR, 'b') * PSCALE;
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new((const char*)FONT);
    PangoAttribute *attrSize = pango_attr_size_new(BODY_SIZE);
    PangoAttribute *attrColor = pango_attr_foreground_new(r, g, b);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrColor);
    
    // Define title text string
    gtk_label_set_text(GTK_LABEL(label), BODY_TEXT);    
    gtk_label_set_attributes(GTK_LABEL(label), attrList);

    return 0;
}



// Setup notification text box 
int setup_noti_text_box(GtkWidget *box) {

    // Gtk label title and body
    GtkWidget *title = gtk_label_new("");
    GtkWidget *body  = gtk_label_new("");
    
    // Setup title and body
    int title_status = setup_noti_title(title);
    int body_status  = setup_noti_body(body);
    
    // Add widgets to box
    if ( title_status && body_status )
        return 1;
    
    if ( !title_status ) 
        gtk_box_pack_start(GTK_BOX(box), title, 0, 0, 0);
    
    if ( !body_status ) 
        gtk_box_pack_start(GTK_BOX(box), body, 0, 0, 0);
    
    // Set box margins
    gtk_widget_set_margin_start(box, MARGIN);
    gtk_widget_set_margin_end(box, MARGIN);
    gtk_widget_set_margin_top(box, MARGIN);
    gtk_widget_set_margin_bottom(box, MARGIN);
    
    return 0;
}
