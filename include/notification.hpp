/**
 * -----------------------------------------------------------------------------
 * @file notification.hpp
 * @author Gabriel Gonzalez
 * 
 * @note Add a --gravity option to indicate where to put the notification. The
 *       default currently is the top right.
 * 
 * @brief The interface for the Aria notification bubble.
 * -----------------------------------------------------------------------------
 */

#ifndef ARIA_NOTIFICATION_HPP
#define ARIA_NOTIFICATION_HPP

#include "aria.hpp"
#include "commandline.hpp"
#include <gtkmm.h>
#include <string>

ARIA_NAMESPACE

/**
 * @class notification
 * 
 * @brief Control the Aria notification bubble object.
 */
class notification: public Gtk::Window
{
public:
    /**
     * @brief Contruct the notification bubble window, widget containers, and
     *        set up various signals.
     */
    notification();

    /**
     * @brief Build the notification bubble and set all attributes.
     * 
     * @param[in] cli The command line interface, containing all the command
     *                line information.
     */
    int build(commandline::interface& cli);

    /**
     * @brief Display the notification bubble.
     */
    int show(void);

    /**
     * @brief Set the title and body text, as well as their respective fonts and
     *        sizes.
     * 
     * @param[in] title     Title of the notification bubble.
     * @param[in] body      Body of the notification bubble.
     * @param[in] font      Font for both the title and body.
     * @param[in] titlesize Font size of the title.
     * @param[in] bodysize  Font size of the body.
     */
    int set_notify_title_and_body(std::string& title, std::string& body,
                                  std::string& font, std::string& titlesize,
                                  std::string& bodysize);

    /**
     * @brief Set the notification icon.
     * 
     * @param[in] path    The path to the icon.
     * @param[in] spacing The spacing, in pixels, between the icon and text.
     */
    int set_notify_icon(std::string& path, std::string& spacing);

    /**
     * @brief Set the amount of time after which the notification bubble will
     *        disappear.
     * 
     * @param[in] time The amount of time, in seconds.
     */
    int set_notify_time(std::string& time);

    /**
     * @brief Set the notification bubble size (width x height).
     * 
     * @param[in] width  The width of the notification bubble, in pixels.
     * @param[in] height The height of the notification bubble, in pixels.
     */
    int set_notify_size(std::string& width, std::string& height);

    /**
     * @brief Set the notification bubble position on the screen.
     * 
     * @param[in] xpos X-position on the screen.
     * @param[in] ypos Y-position on the screen.
     */
    int set_notify_position(std::string& xpos, std::string& ypos,
                            std::string& gravity);

    /**
     * @brief Set the notification background, foreground, and opacity.
     * 
     * @param[in] background The color of the notification bubble.
     * @param[in] foreground The color of the text.
     * @param[in] opacity    The opacity of the notification bubble.
     */
    int set_notify_color(std::string& background, std::string& foreground,
                         std::string& opacity);

    /**
     * @brief Set the curvature of the corners of the notification bubble.
     * 
     * @param[in] curve The curvature of the corners (in pixels).
     */
    int set_notify_curve(std::string& curve);

    /**
     * @brief Set the margin of the notification bubble.
     * 
     * @param[in] margin  The main margin value, in pixels. If this is set, all
     *                    other margins are set to this value.
     * @param[in] mtop    The margin from the top, in pixels.
     * @param[in] mbottom The margin from the bottom, in pixels.
     * @param[in] mleft   The margin from the left, in pixels.
     * @param[in] mright  The margin from the right, in pixels.
     */
    int set_notify_margin(std::string& margin, std::string& mtop,
                          std::string& mbottom, std::string& mleft,
                          std::string& mright);

protected:
    /**
     * @brief Resize the notification bubble to the desired size, if specified,
     *        or the preferred size, otherwise.
     */
    void resize(void);

    /**
     * @brief Move the notification bubble to the desired position.
     */
    void reposition(void);

    /**
     * @brief Set the title of the notification bubble.
     * 
     * @param[in] title The title of the notification bubble.
     * @param[in] font  The font to use for the title.
     * @param[in] size  The font size for the title.
     */
    int set_title(std::string& title, std::string& font, std::string& size);

    /**
     * @brief Set the title of the notification bubble.
     * 
     * @param[in] body The title of the notification bubble.
     * @param[in] font The font to use for the title.
     * @param[in] size The font size for the title.
     */
    int set_body(std::string& body, std::string& font, std::string& size);

    /**
     * @brief Generically create a label and add it to the notification bubble.
     * 
     * @param[in] text Text for the label.
     * @param[in] font Font for the text.
     * @param[in] size Font size for the text.
     */
    int set_text(std::string text, std::string font, std::string size);

    /**
     * @brief Set the title size.
     * 
     * @param[in] size The title size.
     */
    int set_title_size(std::string& size);

    /**
     * @brief Set the body size.
     * 
     * @param[in] size The body size.
     */
    int set_body_size(std::string& size);

    /**
     * @brief Set the font to use for the text in the notification bubble.
     * 
     * @param[in] font The font to use for the text.
     */
    int set_font(std::string& font);

    /**
     * @brief Set the font size to use for the title or body.
     * 
     * @param[in] key  A string, "title" or "body", indicating to set the font size
     *                 for the title or body.
     * @param[in] size The font size to use.
     */
    int set_font_size(const std::string key, std::string& size);

    /**
     * @brief Set the color of the notification bubble.
     * 
     * @param[in] color The color of the notification bubble.
     */
    int set_background(std::string& color);

    /**
     * @brief Set the text color of the notification bubble.
     * 
     * @param[in] color The color of the notification bubble.
     */
    int set_foreground(std::string& color);

    /**
     * @brief Set the foreground or background color.
     * 
     * @param[in] key   The key to look for in the config file, in the event that
     *                  color is not specified.
     * @param[in] color The background or foreground color to set.
     */
    int set_color(const std::string key, std::string& color);

    /**
     * @brief Set the opacity of the notification bubble.
     * 
     * @param[in] opacity The opacity of the notification bubble.
     */
    int set_opacity(std::string& opacity);

    /**
     * @brief Search the config file for the key, and set value to the one found in
     *        the config file.
     * 
     * @param[in] key   The string to search for in the config file.
     * @param[in] value The value to populate with the one found in the config file.
     */
    int set_from_config(const std::string key, std::string& value);

    /**
     * @brief Parse the color string and convert it to a '#123456' string if the
     *        input is a hex string.
     * 
     * @param[in] color The color hex string.
     */
    std::string fix_color(std::string& color);

    /**
     * @brief Determine the screen resolution for monitor 0 from 
     * 
     * @param[out] width  The width of the monitor.
     * @param[out] height The height of the monitor.
     */
    int get_screen_resolution(int& width, int& height);

    /**
     * @brief Check if the color string is in a hex format.
     * 
     * @param[in] color The color string to check.
     */
    bool is_hex_color(std::string& color);

    /**
     * @brief Draw the notification bubble.
     *
     * @param[in] cr Cairo drawing context.
     */
    virtual bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr);

    /**
     * @brief Set the visual that should be used for the gobj(). Not really sure
     *        what this does.
     *
     * @param[in] previous_screen The previous screen.
     */
    void on_screen_changed(const Glib::RefPtr<Gdk::Screen>& previous_screen);

private:
    /**
     * @brief Cleanup any memory mapped data and gracefully shutdown program.
     * 
     * @param[in] sig The signal that was captured, to cause the cleanup function to
     *                run.
     */
    static void cleanup(int sig);

    /**
     * @brief Main container for the icon and text containers.
     * 
     * @details This is added to the notification object, which is a derived
     *          class of a GtkWindow.
     */
    Gtk::Box bubble_;

    /**
     * @brief Container for the icon.
     */
    Gtk::Box icon_;

    /**
     * @brief Container for the title and body text.
     */
    Gtk::Box text_;

    /**
     * @brief Background color of the notification bubble.
     * 
     * @details Using this type, instead of a string, allows for using the
     *          methods: get_red(), get_green(), and get_blue() which is used
     *          when drawing the notification bubble.
     */
    Gdk::RGBA background_;

    /**
     * @brief Width of the notification bubble.
     */
    int width_;

    /**
     * @brief Height of the notification bubble.
     */
    int height_;

    /**
     * @brief X-position, on screen, of where the notification bubble will be
     *        displayed.
     */
    int xpos_;

    /**
     * @brief Y-position, on screen, of where the notification bubble will be
     *        displayed.
     */
    int ypos_;

    /**
     * @brief Gravity of which corner to consider the origin (0, 0) point.
     */
    std::string gravity_;

    /**
     * @brief Curvature of the corners on the notification bubble.
     */
    int curve_;
};

ARIA_NAMESPACE_END

#endif /* ARIA_NOTIFICATION_HPP */
