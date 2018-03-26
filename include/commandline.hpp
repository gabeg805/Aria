/**
 * @file commandline.hpp
 * @author Gabriel Gonzalez
 * 
 * @bug It looks like the list argument can act as an optional argument as well.
 * 
 * @brief A command line interface utility to parse options, print usage, and
 *        notify the user when an error occurs.
 */

#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @namespace commandline
 * 
 * @brief Command line interface utility.
 */
namespace commandline
{
    const size_t kArgumentNameLength = 32;

    /**
     * @enum The type of argument for an option.
     */
    enum argument_t
    {
        no_argument,       /** No argument. */
        required_argument, /** One required argument after an option. */
        optional_argument, /** An optional argument after an option. */
        list_argument      /** One or more arguments after an option. */
    };

    /**
     * @struct option
     * 
     * @brief A structure containing all the information one needs to know about
     *        an option.
     * 
     * @details All the information about an option, such as: the short form,
     *          long form, argument name, argument type, and a description of
     *          the option.
     */
    struct option
    {
        std::string shortopt; /** Short form of the option. */
        std::string longopt;  /** Long form of the option. */
        std::string name;     /** Name of the argument. */
        argument_t  argument; /** Type of argument. */
        std::string desc;     /** Description of the option. */
    };

    /**
     * @brief Type name for an option.
     */
    typedef struct option option_t;

    /**
     * @brief Type name for a list of all options in a program.
     */
    typedef std::vector<option_t> optlist_t;

    /**
     * @brief Type name for list of all entered options and their respective
     *        values.
     */
    typedef std::unordered_map<std::string, std::vector<std::string>> keyval_t;

    /**
     * @class interface
     * 
     * @brief Interface to the command line, where a user will parse, print
     *        program usage, etc.
     */
    class interface
    {
    public:
        /**
         * @brief Construct the command line interface.
         * 
         * @param[in] options List of all command line options for the program.
         */
        explicit interface(const optlist_t options);

        /**
         * @brief Print the program usage message.
         */
        void usage(void);

        /**
         * @brief Parse the list of arguments given on the command line.
         * 
         * @param[in] argv List of command line arguments (from main function).
         */
        void parse(char** argv);

        /**
         * @brief Print the command line options that have been entered.
         */
        void print(void);

        /**
         * @brief Set the value for the given option.
         * 
         * @param opt   An option who's value to set.
         * @param value The value to set for the given option.
         */
        int set(std::string opt, std::string value);


        std::string get(std::string opt);
        bool has(std::string opt);

    protected:
        char** parse_argument(const option_t* option, char** arg, bool& listflag);
        bool parse_list_argument(char** arg, std::string key, bool& listflag);
        const option_t* find_option(std::string opt);
        std::string extract(std::string opt, int field);
        std::string extract_option(std::string opt);
        std::string extract_value(std::string opt);
        std::string to_short_option(std::string opt);
        std::string to_long_option(std::string opt);
        std::string to_key(std::string input);
        bool is_option(std::string opt);
        bool is_option(const option_t* option, std::string opt);
        bool is_short_option(std::string opt);
        bool is_short_option(const option_t* option, std::string opt);
        bool is_long_option(std::string opt);
        bool is_long_option(const option_t* option, std::string opt);

        const optlist_t m_options;
        keyval_t m_table;
    };

}

#endif /* COMMAND_LINE_HPP */
