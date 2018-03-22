/* *****************************************************************************
 * 
 * Name:    arianotification.h
 * Class:   <arianotification>
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
#ifndef ARIA_NOTIFICATION_HPP
#define ARIA_NOTIFICATION_HPP

/* Includes */
#include "ariadef.h"
#include "ariacommandline.h"
#include <gtkmm.h>
#include <string>

ARIA_NAMESPACE

/**
 * Aria notification bubble object.
 */
class notification: public Gtk::Window
{
public:
    notification();

    int build(commandline::interface& cli);
    int show(void);

protected:
    virtual bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr);
    void on_screen_changed(const Glib::RefPtr<Gdk::Screen>& previous_screen);

private:
    int set_title(std::string title, std::string font, std::string size);
    int set_body(std::string title, std::string font, std::string size);
    int set_text(std::string title, std::string font, std::string size);
    int set_background(std::string color);
    int set_foreground(std::string color);
    int set_margin(std::string margin);
    int set_opacity(std::string opacity);
    int set_time(std::string time);
    int set_size(std::string width, std::string height);
    int set_position(std::string xpos, std::string ypos);
    int resize(void);
    int reposition(void);
    static void cleanup(int sig);

    Gtk::Box bubble;
    std::string m_background;
    std::string m_opacity;
    int m_width;
    int m_height;
    int m_xpos;
    int m_ypos;
};

ARIA_NAMESPACE_END

#endif /* ARIA_NOTIFICATION_HPP */
