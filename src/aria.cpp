/**
 * @file aria.cpp
 * @author Gabriel Gonzalez
 * 
 * @section LICENSE
 * 
 * See LICENSE file.
 * 
 * @section DESCRIPTION
 * 
 * @details The Aria notification bubble is a highly customizable program
 *          wherein a user is able to configure settings such as:
 * 
 *            - Title and body
 *            - Font
 *            - Font size for the title and body
 *            - Icon
 *            - Amount of time to display the notification
 *            - X and Y-position on the screen
 *            - Width and height of the notification bubble
 *            - Background and foreground color
 *            - Opacity of the notification bubble
 *            - Curvature of the corners of the notification bubble
 *            - Margin (top, bottom, left and right)
 *            - Spacing between the icon and the text
 * 
 *          Much of the above settings are defined in the configuration and, as
 *          a result, do not need to be specified.
 */

#include "commandline.hpp"
#include "notification.hpp"
#include <gtkmm.h>

/**
 * @brief Create and display the Aria notification bubble.
 * 
 * @return 0 on success. Any other value is an error state.
 */
int main(int argc, char** argv)
{
    /* Command line options */
    commandline::optlist_t options{
        {"-h",  "--help",          "",            commandline::no_argument,       "Print program usage."},
        {"-t",  "--title",         "title",       commandline::required_argument, "Title of the notification."},
        {"-b",  "--body",          "body",        commandline::required_argument, "Body of the notification."},
        {"-i",  "--icon",          "path",        commandline::required_argument, "Icon to display next to the text."},
        {"-T",  "--time",          "time",        commandline::required_argument, "Amount of time to display the notification. [Default: 2s]"},
        {"-X",  "--xpos",          "pos",         commandline::required_argument, "X-coordinate of where to put the notification on the screen."},
        {"-Y",  "--ypos",          "pos",         commandline::required_argument, "Y-coordinate of where to put the notification on the screen."},
        {"-W",  "--width",         "width",       commandline::required_argument, "Width of the notification."},
        {"-H",  "--height",        "height",      commandline::required_argument, "Height of the notification."},
        {"-o",  "--opacity",       "opacity",     commandline::required_argument, "Opacity of the notification. [Default: 0.5]"},
        {"-bg", "--background",    "color",       commandline::required_argument, "Background color. [Default: 0xffa5d0]"},
        {"-fg", "--foreground",    "color",       commandline::required_argument, "Foreground color. [Default: 0xffffff]"},
        {"-c",  "--curve",         "curve",       commandline::required_argument, "Curvature to give corners of the notification bubble. [Default: 20]"},
        {"-m",  "--margin",        "margin",      commandline::required_argument, "Margin all around the notification."},
        {"-mt", "--margin-top",    "margin",      commandline::required_argument, "Top margin of the notification."},
        {"-mb", "--margin-bottom", "margin",      commandline::required_argument, "Bottom margin of the notification."},
        {"-ml", "--margin-left",   "margin",      commandline::required_argument, "Left margin of the notification."},
        {"-mr", "--margin-right",  "margin",      commandline::required_argument, "Right margin of the notification."},
        {"-s",  "--icon-text-spacing", "spacing", commandline::required_argument, "Spacing between the icon and notification bubble text."},
        {"-f",  "--font",          "font",        commandline::required_argument, "Font to display text in. [Default: Dejavu Sans]"},
        {"-ts", "--title-size",    "size",        commandline::required_argument, "Size of title text. [Default: 16]"},
        {"-bs", "--body-size",     "size",        commandline::required_argument, "Size of body text. [Default: 12]"},
    };

    /* Process command line arguments */
    commandline::interface cli(options);
    cli.parse(argv);

    /* Build notification bubble */
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("");
    aria::notification Aria;
    int status;
    if ((status=Aria.build(cli)) != 0) {
        return status;
    }
    if ((status=Aria.show()) != 0) {
        return status;
    }

    return app->run(Aria);
}
