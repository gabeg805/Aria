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
#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

/* Declares */
static std::string attr[15][2] = {
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

const std::string file = "/home/gabeg/scripts/programs/aria/aria.conf";

/* **********************************************
 * ***** INITIALIZE THE NOTIFICATION BUBBLE *****
 * **********************************************
 */

int AriaAttribute::init(char **argv)
{
    set("program", *argv);
    std::string str;

    while ( *++argv != NULL ) {
        str = *argv++;

        if ( (str.compare("-h") == 0) || (str.compare("--help") == 0) )
            return -1;
        else if ( (str.compare("-t") == 0) || (str.compare("--title") == 0) )
            set("title", *argv);
        else if ( (str.compare("-b") == 0) || (str.compare("--body") == 0) )
            set("body", *argv);
        else if ( (str.compare("-f") == 0) || (str.compare("--font") == 0) )
            set("font", *argv);
        else if ( (str.compare("-w") == 0) || (str.compare("--width") == 0) )
            set("width", *argv);
        else if ( (str.compare("-h") == 0) || (str.compare("--height") == 0) )
            set("height", *argv);
        else if ( (str.compare("-x") == 0) || (str.compare("--xpos") == 0) )
            set("xpos", *argv);
        else if ( (str.compare("-y") == 0) || (str.compare("--ypos") == 0) )
            set("ypos", *argv);
        else if ( str.compare("--time") == 0 )
            set("timer", *argv);
        else if ( (str.compare("-o") == 0) || (str.compare("--opacity") == 0) ) 
            set("opacity", *argv);
        else if ( (str.compare("-m") == 0) || (str.compare("--margin") == 0) ) 
            set("margin", *argv);
        else if ( (str.compare("-ts") == 0) || (str.compare("--title-size") == 0) ) 
            set("title_size", *argv);
        else if ( (str.compare("-bs") == 0) || (str.compare("--body-size") == 0) ) 
            set("body_size", *argv);
        else if ( (str.compare("-bg") == 0) || (str.compare("--background") == 0) ) 
            set("background", *argv);
        else if ( (str.compare("-fg") == 0) || (str.compare("--foreground") == 0) ) 
            set("foreground", *argv);
        else
            continue;
    }

    set_defaults();
    return 0;
}

/* ***************************************
 * ***** SET NOTIFICATION ATTRIBUTES *****
 * ***************************************
 */

int AriaAttribute::set(std::string key, std::string val)
{
    size_t len = sizeof(attr) / sizeof(attr[0]);
    size_t i;
    for ( i = 0; i < len; ++i )
        if ( attr[i][0].compare(key) == 0 ) {
            attr[i][1] = val;
            return 0;
        }
    return -1;
}

int AriaAttribute::set(size_t key, std::string val)
{
    size_t len = sizeof(attr) / sizeof(attr[0]);
    size_t i;
    for ( i = 0; i < len; ++i )
        if ( i == key ) {
            attr[i][1] = val;
            return 0;
        }
    return -1;
}

int AriaAttribute::set_defaults(void)
{
    std::ifstream is(file.c_str(), std::ifstream::in);
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
        if ( !get(setting).empty() )
            continue;

        /* Return value pair */
        len       = line.length();
        remainder = line.substr(loc+1, len);
        pos       = remainder.find_first_not_of(" \t");
        if ( pos < 0 )
            continue;
        else
            set(setting, remainder.substr(pos, len));
    }

    return 0;
}

/* ***************************************
 * ***** GET NOTIFICATION ATTRIBUTES *****
 * ***************************************
 */

std::string AriaAttribute::get(std::string key)
{
    size_t len = sizeof(attr) / sizeof(attr[0]);
    size_t i;
    for ( i = 0; i < len; ++i )
        if ( attr[i][0].compare(key) == 0 )
            return attr[i][1];
    return "";
}

std::string AriaAttribute::get(size_t key)
{
    size_t len = sizeof(attr) / sizeof(attr[0]);
    size_t i;
    for ( i = 0; i < len; ++i )
        if ( i == key )
            return attr[i][1];
    return "";
}

/* *******************************
 * ***** PRINT ATTRIBUTE MAP *****
 * *******************************
 */

void AriaAttribute::print(void)
{
    size_t len = sizeof(attr) / sizeof(attr[0]);
    size_t i;

    for ( i = 0; i < len; ++i )
        std::cout << "i: " << i 
                  << " | attr0: " << attr[i][0]
                  << " | attr1: " << attr[i][1] << std::endl;
}
