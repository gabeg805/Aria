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
#include "commandline.h"
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

    int build(commandline::values cli);
    int show(void);

private:
    int set_title(std::string title, std::string font, std::string size);
    int set_body(std::string title, std::string font, std::string size);
    int set_text(std::string title, std::string font, std::string size);
    int set_background(std::string color);
    int set_foreground(std::string color);
    int set_margin(std::string margin);
    int set_time(std::string time);
    int set_size(std::string width, std::string height);
    int set_position(std::string xpos, std::string ypos);
    int resize(void);
    int reposition(void);
    static void cleanup(int sig);

    Gtk::Box bubble;
    std::string m_width;
    std::string m_height;
    std::string m_xpos;
    std::string m_ypos;
};

#endif /* ARIA_CORE_INCLUDE_ARIANOTIFY_H */
