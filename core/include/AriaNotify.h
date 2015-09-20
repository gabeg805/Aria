/* *****************************************************************************
 * 
 * Name:    AriaNotify.h
 * Class:   <AriaNotify>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: The Aria Notification Bubble.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

// Header guard
#ifndef ARIA_NOTIFICATION_H
#define ARIA_NOTIFICATION_H

/* Includes */
#include <gtkmm.h>
#include <string>

/* Classes */
class AriaNotify:
    public Gtk::Window
{
public:
    AriaNotify();

    void init(char **argv);
    void show();
    void position(void);
    bool timeout();

    void set_background(void);
    void set_foreground(void);
    void set_size(void);
    void set_margin(void);
    void set_notify_text(std::string key);
    void set_title(void);
    void set_body(void);
    void set_timer(void);

    int get_xpos(void);
    int get_ypos(void);
    int get_width(void);
    int get_height(void);
    int get_screen_size(void);

    Gtk::Box      bubble;
};

#endif /* ARIA_NOTIFICATION_BUBBLE_H */
