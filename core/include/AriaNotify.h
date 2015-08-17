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

    void usage();
    void init(int argc, char** argv);
    void create();
    bool timeout();

    void check_notify_text(void);
    void set_background(void);
    void set_foreground(void);
    void set_size(void);
    void set_title(void);
    void set_margin(void);
    void set_notify_text(std::string key);
    void set_notify_title_text(void);
    void set_notify_body_text(void);
    void set_timer(void);
    void set_position(void);

    Gtk::Box      bubble;
};

#endif /* ARIA_NOTIFICATION_BUBBLE_H */
