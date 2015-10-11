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

/* Header guard */
#ifndef ARIA_CORE_INCLUDE_ARIANOTIFY_H
#define ARIA_CORE_INCLUDE_ARIANOTIFY_H

/* Includes */
#include <gtkmm.h>
#include <string>

/**
 * @brief Aria notification bubble object.
 * 
 * @details Handles the initialization, creation, and displaying of the
 *          notification bubble.
 */
class AriaNotify:
    public Gtk::Window
{
public:
    AriaNotify();

    void init(char **argv);
    void show(void);
    void position(void);
    static void cleanup(int sig);

    void set_title(void);
    void set_body(void);
    void set_notify_text(std::string field);
    void set_background(void);
    void set_foreground(void);
    void set_margin(void);
    void set_size(void);
    void set_timer(void);

    Gtk::Box      bubble;
};

#endif /* ARIA_CORE_INCLUDE_ARIANOTIFY_H */
