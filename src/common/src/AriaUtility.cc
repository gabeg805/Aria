/* *****************************************************************************
 * 
 * Name:    AriaUtility.cc
 * Class:   <AriaUtility>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Aria Notification Bubble utilities.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "AriaUtility.h"
#include "AriaAttribute.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

/* ************************************************************************** */
/**
 * @brief Print program usage.
 */
void AriaUtility::usage(void)
{
    static std::string prog = AriaAttribute::getstr("program");
    std::cout << "Usage: " << prog << " [option] <argument>\n"
              << "\n"
              << "Options:\n"
              << "    -h, --help                   Print program usage message\n"
              << "    -t, --title <text>           Title text\n"
              << "    -b, --body <text>            Body text\n"
              << "    -w, --width <size>           Width size\n"
              << "    -h, --height <size>          Height size\n"
              << "    -x, --xpos <position>        Location in the x-direction to move\n"
              << "    -y, --ypos <position>        Location in the y-direction to move\n"
              << "    --time <seconds>             Amount of time (in seconds) to display\n"
              << "    -o, --opacity <value>        Opacity (0 <= value <= 1)\n"
              << "    -m, --margin <value>         Box margins(0 < value)\n"
              << "    -f, --font <font>            Text font\n"
              << "    -d, --delay <seconds>        Amount of time to delay between program calls.\n"
              << "    -ts, --title-size <size>     Size of text for the title\n"
              << "    -bs, --body-size <size>      Size of text for the body\n"
              << "    -bg, --background <color>    Color of the notification bubble\n"
              << "    -fg, --foreground <color>    Color of the text\n"
              << "\n"
              << "Arguments:\n"
              << "    <text>                       Text to display\n"
              << "    <size>                       Size of the notification bubble\n"
              << "    <position>                   X-Y position of the notitification bubble\n"
              << "    <value>                      An integer or double value\n"
              << "    <font>                       Text font\n"
              << "    <size>                       Text size\n"
              << "    <color>                      Hexadecimal color"
              << std::endl;
}

/* ************************************************************************** */
/**
 * @brief Print error message and exit.
 * 
 * @param str the desired error message to print out.
 */
void AriaUtility::errprint(std::string str)
{
    static std::string prog = AriaAttribute::getstr("program");
    std::cout << prog << ": " << str << std::endl;
}

/* ************************************************************************** */
/**
 * @brief Print error message and exit.
 * 
 * @param str the desired error message to print out.
 * 
 * @param err the errno, set by the previous command.
 */
void AriaUtility::errprint(std::string str, int err)
{
    static std::string prog = AriaAttribute::getstr("program");
    std::cout << prog << ": " << str << ": " << std::strerror(err) << std::endl;
}
