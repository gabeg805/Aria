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
#include "ariadef.hpp"
#include "ariacommandline.hpp"
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
    int set_notify_title_and_body(std::string& title, std::string& body,
                                  std::string& font, std::string& titlesize,
                                  std::string& bodysize);
    int set_notify_time(std::string& time);
    int set_notify_size(std::string& width, std::string& height);
    int set_notify_position(std::string& xpos, std::string& ypos);
    int set_notify_color(std::string& background, std::string& foreground,
                         std::string& opacity);
    int set_notify_curve(std::string& curve);
    int set_notify_margin(std::string& margin, std::string& mtop,
                          std::string& mbottom, std::string& mleft,
                          std::string& mright);
protected:
    int resize(void);
    int reposition(void);
    int set_title(std::string& title, std::string& font, std::string& size);
    int set_body(std::string& body, std::string& font, std::string& size);
    int set_text(std::string text, std::string font, std::string size);
    int set_title_size(std::string& size);
    int set_body_size(std::string& size);
    int set_font(std::string& font);
    int set_font_size(const std::string key, std::string& size);
    int set_background(std::string& color);
    int set_foreground(std::string& color);
    int set_color(const std::string key, std::string& color);
    int set_opacity(std::string& opacity);
    int set_from_config(const std::string key, std::string& value);
    std::string fix_color(std::string& color);
    bool is_hex_color(std::string& color);

    virtual bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr);
    void on_screen_changed(const Glib::RefPtr<Gdk::Screen>& previous_screen);

private:
    static void cleanup(int sig);

    Gtk::Box m_bubble;
    Gtk::Box m_icon;
    Gtk::Box m_text;
    std::string m_background;

    std::string m_opacity;

    int m_width;
    int m_height;
    int m_xpos;
    int m_ypos;
    int m_curve;
    int m_margin_top;
    int m_margin_bottom;
    int m_margin_left;
    int m_margin_right;
};

ARIA_NAMESPACE_END

#endif /* ARIA_NOTIFICATION_HPP */
