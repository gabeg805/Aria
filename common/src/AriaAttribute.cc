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
const std::string CONF_FILE = "/home/gabeg/scripts/programs/aria/aria.conf";
static std::string ATTR[16][2] = {
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
    {"title-size", "16"},
    {"body-size",  "12"},
    {"background", "#ffa500"},
    {"foreground", "#ffffff"},
    {"program",    "aria"}
};

/* ************************************************************************** */
/**
 * @brief Initialize the Aria attribute data structure by populating some user
 *        defined pieces.
 * 
 * @details Essentially a specialized getopt. Loop through the argument vector
 *          searching for user defined pieces and populate the data structure
 *          when a new piece is found. When all user defined pieces are
 *          specified, fill in the remainder with defaults.
 * 
 * @params argv the command line argument vector.
 */
int AriaAttribute::init(char **argv)
{
    AriaAttribute::setstr("program", *argv);
    std::string str;

    while ( *++argv != NULL ) {
        str = *argv++;

        if ( (str.compare("-h") == 0) || (str.compare("--help") == 0) )
            return -1;
        else if ( (str.compare("-t") == 0) || (str.compare("--title") == 0) )
            AriaAttribute::setstr("title", *argv);
        else if ( (str.compare("-b") == 0) || (str.compare("--body") == 0) )
            AriaAttribute::setstr("body", *argv);
        else if ( (str.compare("-f") == 0) || (str.compare("--font") == 0) )
            AriaAttribute::setstr("font", *argv);
        else if ( (str.compare("-w") == 0) || (str.compare("--width") == 0) )
            AriaAttribute::setstr("width", *argv);
        else if ( (str.compare("-h") == 0) || (str.compare("--height") == 0) )
            AriaAttribute::setstr("height", *argv);
        else if ( (str.compare("-x") == 0) || (str.compare("--xpos") == 0) )
            AriaAttribute::setstr("xpos", *argv);
        else if ( (str.compare("-y") == 0) || (str.compare("--ypos") == 0) )
            AriaAttribute::setstr("ypos", *argv);
        else if ( str.compare("--time") == 0 )
            AriaAttribute::setstr("timer", *argv);
        else if ( (str.compare("-o") == 0) || (str.compare("--opacity") == 0) ) 
            AriaAttribute::setstr("opacity", *argv);
        else if ( (str.compare("-m") == 0) || (str.compare("--margin") == 0) ) 
            AriaAttribute::setstr("margin", *argv);
        else if ( (str.compare("-ts") == 0) || (str.compare("--title-size") == 0) ) 
            AriaAttribute::setstr("title_size", *argv);
        else if ( (str.compare("-bs") == 0) || (str.compare("--body-size") == 0) ) 
            AriaAttribute::setstr("body_size", *argv);
        else if ( (str.compare("-bg") == 0) || (str.compare("--background") == 0) ) 
            AriaAttribute::setstr("background", *argv);
        else if ( (str.compare("-fg") == 0) || (str.compare("--foreground") == 0) ) 
            AriaAttribute::setstr("foreground", *argv);
        else
            continue;
    }

    AriaAttribute::setdef();
    return 0;
}

/* ************************************************************************** */
/**
 * @brief Set values in the attribute data structure using a key-value pairing
 *        system.
 * 
 * @params key the unique identifier in the attribute data structure which
 *             points to a value.
 * @params val the value which the key points to.
 */
int AriaAttribute::setstr(std::string key, std::string val)
{
    size_t len = sizeof(ATTR) / sizeof(ATTR[0]);
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
 * @params key the unique identifier in the attribute data structure which
 *             points to a value.
 * @params val the value which the key points to.
 */
int AriaAttribute::setint(std::string key, int val)
{
    size_t            len = sizeof(ATTR) / sizeof(ATTR[0]);
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
int AriaAttribute::setdef(void)
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
 * @brief Return the value in the attribute data structure that is pointed to by
 *        the given key.
 * 
 * @params key the unique identifier in the attribute data structure which
 *             points to a value.
 */
std::string AriaAttribute::getstr(std::string key)
{
    size_t len = sizeof(ATTR) / sizeof(ATTR[0]);
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
 * @params key the unique identifier in the attribute data structure which
 *             points to a value.
 */
int AriaAttribute::getint(std::string key)
{
    return atoi(AriaAttribute::getstr(key).c_str());
}

/* ************************************************************************** */
/**
 * @brief Print every key-value pair in the attribute data structure.
 */
void AriaAttribute::print(void)
{
    size_t len = sizeof(ATTR) / sizeof(ATTR[0]);
    size_t i;

    for ( i = 0; i < len; ++i )
        std::cout << "i: " << i 
                  << " | attr0: " << ATTR[i][0]
                  << " | attr1: " << ATTR[i][1] << std::endl;
}
