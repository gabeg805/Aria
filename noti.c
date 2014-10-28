// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     noti.cpp
// 
// 
// SYNTAX: 
// 
//     ./noti
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
//     Unknown
// 
// 
// FUNCTIONS:
// 
//     main - the main function
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Objective
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 28 2014 <> created
// 
//     gabeg Oct 28 2014 <> Rounded the edges of the notification bubble.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SCALE   1024
#define PSCALE    65535 / 255
#define CSCALE    1.0 / 255.0

// Declares
int WIDTH;
int HEIGHT;
char *TITLE_TEXT;
char *BODY_TEXT;

int XPOS         = 30;
int YPOS         = 30;
double OPACITY   = 0.75;
int MARGIN       = 10;
int TIMER        = 3000;
char *FG_COLOR   = "#000000";
char *BG_COLOR   = "#EEB111";
const char *FONT = "DejaVu Sans";
int TITLE_SIZE   = 18*SCALE;
int BODY_SIZE    = 12*SCALE;



// /////////////////////////////////
// ///// READ INPUT PARAMETERS /////
// /////////////////////////////////

// Read program input parameters
void setup_parameters(char *argv[]) {
    
    // Determine which parameters were defined
    int i = 0;
    while ( argv[i+1] != NULL ) {
        ++i;
        
        if       ( strcmp("-t", argv[i]) == 0 ) 
            TITLE_TEXT = argv[i+1];
         else if ( strcmp("-b", argv[i]) == 0 ) 
             BODY_TEXT = argv[i+1];
         else if ( strcmp("-x", argv[i]) == 0 ) 
             XPOS = atoi(argv[i+1]);
         else if ( strcmp("-y", argv[i]) == 0 ) 
             YPOS = atoi(argv[i+1]);
         else if ( strcmp("-o", argv[i]) == 0 ) 
             OPACITY = atof(argv[i+1]); 
         else if ( strcmp("-m", argv[i]) == 0 ) 
             MARGIN = atoi(argv[i+1]); 
         else if ( strcmp("--time", argv[i]) == 0 ) 
             TIMER = atoi(argv[i+1])*1000;
         else if ( strcmp("--font", argv[i]) == 0 ) 
             FONT = argv[i+1];
         else if ( strcmp("--tsize", argv[i]) == 0 ) 
             TITLE_SIZE = atoi(argv[i+1]) * SCALE;
         else if ( strcmp("--bsize", argv[i]) == 0 ) 
             BODY_SIZE = atoi(argv[i+1]) * SCALE;
         else if ( strcmp("-fg", argv[i]) == 0 ) 
             FG_COLOR = argv[i+1]; 
         else if ( strcmp("-bg", argv[i]) == 0 ) 
             BG_COLOR = argv[i+1]; 
         else  
             continue;
        
        ++i;
    }
}



// Close notification bubble
gboolean quit(GtkWindow *window) {
    gtk_main_quit();
    return TRUE;
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



// //////////////////////
// ///// DRAW FRAME /////
// //////////////////////

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

// Draw the login frame
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




// ////////////////////////////////////////////////////
// ///// SETUP WINDOW, TITLE, AND BODY ATTRIBUTES /////
// ////////////////////////////////////////////////////

// Setup window attributes
void setup_noti_win(GtkWidget *window, GtkWidget *box) {

    // Set window attributes
    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_NORTH_EAST);
    
    // Enable window transparency
    enable_transparency(window);
    
    // GTK signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_timeout_add(TIMER, (GSourceFunc)quit, window);         
}



// Setup title text attributes
void setup_noti_title(GtkWidget *label) {
    
    // Make sure title text was set
    if ( TITLE_TEXT == NULL ) {
        printf("%s\n", "ERROR: You did not set title text.");
        return;
    }

    // Get color to set text
    int r = get_color(FG_COLOR, 'r') * PSCALE;
    int g = get_color(FG_COLOR, 'g') * PSCALE;
    int b = get_color(FG_COLOR, 'b') * PSCALE;
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(FONT);
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
}



// Setup body text attributes
void setup_noti_body(GtkWidget *label) {
    
    // Make sure title text was set
    if ( BODY_TEXT == NULL ) {
        printf("%s\n", "ERROR: You did not set body text.");
        return;
    }
    
    // Get color to set text
    int r = get_color(FG_COLOR, 'r') * PSCALE;
    int g = get_color(FG_COLOR, 'g') * PSCALE;
    int b = get_color(FG_COLOR, 'b') * PSCALE;
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(FONT);
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
}



// Setup notification text box 
void setup_noti_text_box(GtkWidget *box, GtkWidget *first, GtkWidget *second) {
    
    // Add widgets to box
    gtk_box_pack_start(GTK_BOX(box), first, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(box), second, 0, 0, 0);
    
    // Set box margins
    gtk_widget_set_margin_start(box, MARGIN);
    gtk_widget_set_margin_end(box, MARGIN);
    gtk_widget_set_margin_top(box, MARGIN);
    gtk_widget_set_margin_bottom(box, MARGIN);
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



// ///////////////////////////////////////
// ///// DISPLAY NOTIFICATION BUBBLE /////
// ///////////////////////////////////////

// Run notification bubble in background
int main(int argc, char *argv[]) {
    
    // Initialize gtk toolkit
    gtk_init(&argc, &argv);
    
    GtkWidget *window    = gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget *frame     = gtk_drawing_area_new();
    GtkWidget *frame_box = gtk_overlay_new();
    GtkWidget *text_box  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *one       = gtk_label_new("");
    GtkWidget *two       = gtk_label_new("");
    
    // Define global variables
    setup_parameters(argv);
    
    // Setup notification title and body
    setup_noti_title(one);
    setup_noti_body(two);
    setup_noti_text_box(text_box, one, two);
    
    // Setup label window attributes
    setup_noti_win(window, text_box);
    setup_noti_win_dims(window);
    
    GtkWidget *pat = gtk_label_new("");
    GtkWidget *chan = gtk_label_new("");
    GtkWidget *lebox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    // Setup notification title and body
    setup_noti_title(pat);
    setup_noti_body(chan);
    setup_noti_text_box(lebox, pat, chan);
    
    gtk_container_add(GTK_CONTAINER(frame_box), frame);
    gtk_container_remove(GTK_CONTAINER(window), text_box);        
    gtk_overlay_add_overlay(GTK_OVERLAY(frame_box), lebox);
    gtk_container_add(GTK_CONTAINER(window), frame_box);
    
    // Start timer
    g_signal_connect(G_OBJECT(frame), "draw", G_CALLBACK(draw_window), NULL);
    
    // Display the label
    gtk_widget_show_all(window);
    
    // Run the gtk loop
    gtk_main();
    
    return 0;
}
