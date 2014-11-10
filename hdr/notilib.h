// Header guard
#ifndef NOTILIB_H
#define NOTILIB_H

// Includes
#include <gtk/gtk.h>
#include <cairo.h>

// Public functions
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

#endif
