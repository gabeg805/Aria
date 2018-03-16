/* *****************************************************************************
 * 
 * Name:    AriaAttribute.cc
 * Class:   <AriaAttribute>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Manage Aria Notification Bubble attributes.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "AriaAttribute.h"
#include <X11/Xlib.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/* Declares */
static const std::string CONF_FILE = "/home/gabeg/scripts/programs/aria/src/common/aria.conf";
static std::string ATTR[17][2]     = {
    {"screen",     ""},
    {"title",      ""},
    {"body",       ""},
    {"font",       "DejaVu"},
    {"width",      "0"},
    {"height",     "0"},
    {"xpos",       "0"},
    {"ypos",       "0"},
    {"timer",      "2"},
    {"opacity",    "1"},
    {"margin",     "7"},
    {"delay",      "0"},
    {"title-size", "16"},
    {"body-size",  "12"},
    {"background", "#ffa500"},
    {"foreground", "#ffffff"},
    {"program",    "aria"}
};

/* ************************************************************************** */
/**
 * @brief Return the value in the attribute data structure that is pointed to by
 *        the given key.
 * 
 * @param key the unique identifier in the attribute data structure which
 *             points to a value.
 */
std::string AriaAttribute::getstr(std::string key)
{
    size_t len = ARRAY_SIZE(ATTR);
    size_t i;
    for ( i = 0; i < len; ++i )
        if ( ATTR[i][0].compare(key) == 0 )
            return ATTR[i][1];
    return "";
}

/* ************************************************************************** */
/**
 * @brief Return the value in the attribute data structure that is pointed to by
 *        the given key.
 * 
 * @param key the unique identifier in the attribute data structure which
 *             points to a value.
 */
int AriaAttribute::getint(std::string key)
{
    return atoi(AriaAttribute::getstr(key).c_str());
}

/* ************************************************************************** */
/**
 * @brief Set values in the attribute data structure using a key-value pairing
 *        system.
 * 
 * @param key the unique identifier in the attribute data structure which
 *             points to a value.
 * @param val the value which the key points to.
 */
int AriaAttribute::setstr(std::string key, std::string val)
{
    size_t len = ARRAY_SIZE(ATTR);
    size_t i;
    for ( i = 0; i < len; ++i )
        if ( ATTR[i][0].compare(key) == 0 ) {
            ATTR[i][1] = val;
            return 0;
        }
    return -1;
}

/* ************************************************************************** */
/**
 * @brief Set values in the attribute data structure using a key-value pairing
 *        system.
 * 
 * @param key the unique identifier in the attribute data structure which
 *             points to a value.
 * @param val the value which the key points to.
 */
int AriaAttribute::setint(std::string key, int val)
{
    size_t            len = ARRAY_SIZE(ATTR);
    size_t            i;
    std::stringstream ss;
    for ( i = 0; i < len; ++i )
        if ( ATTR[i][0].compare(key) == 0 ) {
            ss << val;
            ATTR[i][1] = ss.str();
            return 0;
        }
    return -1;
}

/* ************************************************************************** */
/**
 * @brief Set default values in the attribute data structure by reading and
 *        parsing the configuration file.
 */
int AriaAttribute::setdefaults(void)
{
    std::ifstream is(CONF_FILE.c_str(), std::ifstream::in);
    std::string   delimiter = ":";
    std::string   line;
    std::string   setting;
    std::string   remainder;
    size_t        loc;
    size_t        len;
    int           pos;

    while ( std::getline(is, line) ) {
        if ( (line.length() > 0) && (line.at(0) == '#') )
            continue;

        /* Check config setting key */
        loc     = line.find(delimiter);
        setting = line.substr(0, loc);
        setting.erase(std::remove(setting.begin(), setting.end(), ' '), setting.end());
        if ( !AriaAttribute::getstr(setting).empty() )
            continue;

        /* Return value pair */
        len       = line.length();
        remainder = line.substr(loc+1, len);
        pos       = remainder.find_first_not_of(" \t");
        if ( pos < 0 )
            continue;
        else
            AriaAttribute::setstr(setting, remainder.substr(pos, len));
    }

    Display          *dpy    = XOpenDisplay(NULL);
    int               screen = DisplayWidth(dpy, DefaultScreen(dpy));
    std::stringstream ss;
    ss << screen;
    AriaAttribute::setstr("screen", ss.str());

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Print every key-value pair in the attribute data structure.
 */
void AriaAttribute::printlist(void)
{
    size_t len = ARRAY_SIZE(ATTR);
    size_t i;

    for ( i = 0; i < len; ++i )
        std::cout << "key: " << ATTR[i][0]
                  << " | value: " << ATTR[i][1] << std::endl;
}
