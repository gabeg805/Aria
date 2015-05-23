// -*-c++-*-
// **********************************************************************************
// 
// Name:    NotificationBubble.h
// Class:   <NotificationBubble>
// Author:  Gabriel Gonzalez
// Email:   gabeg@bu.edu
// License: The MIT License (MIT)
// 
// Description: The Aria notification bubble container skeleton.
//              
// Notes: None.
// 
// **********************************************************************************


// ============
// Header Guard
// ============

#ifndef NOTIFICATIONBUBBLE_H
#define NOTIFICATIONBUBBLE_H


// ========
// Includes
// ========

#include <gtkmm.h>
#include <map>
#include <string>


// =======
// Classes
// =======

class NotificationBubble : public Gtk::Window {
public:
    NotificationBubble();
    
    void usage();
    void init(int argc, char** argv);
    void create();
    bool timeout();
    
    // Container for notification bubble items
    Gtk::Box      bubble;
    Gtk::Label    title;
    Gtk::Label    body;
    
    Pango::FontDescription title_fd;
    Pango::FontDescription body_fd;
    
    std::map<std::string, std::string> attr;
    std::map<std::string, std::string> def;
};

#endif
