/**
 * -----------------------------------------------------------------------------
 * @file commandline.hpp
 * @author Gabriel Gonzalez
 * 
 * @brief A command line interface utility to parse options, print usage, and
 *        notify the user when an error occurs.
 * -----------------------------------------------------------------------------
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
    /**
     * @brief Maximum length of an argument name, when printing out program
     *         usage.
     */
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

        /**
         * @brief Retrieve the value for the given option.
         * 
         * @param[in] opt An option entered in the command line.
         */
        std::string get(std::string opt);

        /**
         * @brief Check if the given option has been entered on the command
         *        line.
         * 
         * @param[in] opt An option entered in the command line.
         */
        bool has(std::string opt);

    protected:
        /**
         * @brief Determine the argument type and, depending on the type, store
         *        the corresponding value.
         * 
         * @param[in]     option   Data structure for an option.
         * @param[in]     arg      Argument list pointer, pointing to the
         *                         current command line option.
         * @param[in/out] listflag Used to set the list flag if a list argument
         *                         is found.
         */
        char** parse_argument(const option_t* option, char** arg, bool& listflag);

        /**
         * @brief Check if there is a list argument, and if there is, store the
         *        argument(s).
         * 
         * @param[in      arg      Argument list pointer, pointing to the
         *                         current argument.
         * @param[in      key      The key used to set a value in m_table.
         * @param[in/out] listflag A flag indicating if the previously found
         *                         option contains list type arguments.
         */
        bool parse_list_argument(char** arg, std::string key, bool& listflag);

        /**
         * @brief Find an option struct that matches the given option string.
         * 
         * @param[in] opt The option string to search for.
         */
        const option_t* find_option(std::string opt);

        /**
         * @brief Extract either the option or value from a long option string.
         *        
         * 
         * @param[in] opt   The long option string of the form
         *                  '--long-option=value'.
         * @param[in] field A field of 1 means the '--long-option' section and a
         *                  field of 2 means the 'value' section.
         */
        std::string extract(std::string opt, int field);

        /**
         * @brief Extract the long option section from a long option string.
         * 
         * @param[in] opt The long option string.
         */
        std::string extract_option(std::string opt);

        /**
         * @brief Extract the value section from a long option string.
         * 
         * @param[in] opt The long option string.
         */
        std::string extract_value(std::string opt);

        /**
         * @brief Convert an option string, long or short, to a short option.
         * 
         * @param[in] opt An option string.
         */
        std::string to_short_option(std::string opt);

        /**
         * @brief Convert an option string, long or short, to a long option.
         * 
         * @param[in] opt An option string.
         */
        std::string to_long_option(std::string opt);

        /**
         * @brief Convert input option to a key string. This means
         *        '--long-option' is converted to 'long-option' and if there is
         *        no long option, then '-short' is converted to 'short'.
         * 
         * @param[in] input The option string to convert a key.
         */
        std::string to_key(std::string input);

        /**
         * @brief Check if the given option is a valid short or long command
         *        line option.
         * 
         * @param[in] opt An option string.
         */
        bool is_option(std::string opt);

        /**
         * @brief Check if the given option is a valid short or long command
         *        line option.
         * 
         * @param[in] option An option struct.
         * @param[in] opt    An option string.
         */
        bool is_option(const option_t* option, std::string opt);

        /**
         * @brief Check if the given option is a valid short command line
         *        option.
         * 
         * @param[in] opt An option string.
         */
        bool is_short_option(std::string opt);

        /**
         * @brief Check if the given option is a valid short command line
         *        option.
         * 
         * @param[in] option An option struct.
         * @param[in] opt    An option string.
         */
        bool is_short_option(const option_t* option, std::string opt);

        /**
         * @brief Check if the given option is a valid long command line option.
         * 
         * @param[in] opt An option string.
         */
        bool is_long_option(std::string opt);

        /**
         * @brief Check if the given option is a valid long command line option.
         * 
         * @param[in] option An option struct.
         * @param[in] opt    An option string.
         */
        bool is_long_option(const option_t* option, std::string opt);

        /**
         * @brief List of all possible options that can be supplied to the
         *        program.
         */
        const optlist_t m_options;

        /**
         * @brief A perfect hash table containing the options that were supplied
         *        in the command line, and their corresponding values.

         * @details The keys for this hash table are the option strings that are
         *          supplied, e.g. '--long-option' or '-short', but without the
         *          leading dash(es). Long options are used as the default key,
         *          but if no long option exists, it falls back to the short
         *          option.
         */
        keyval_t m_table;
    };

}

#endif /* COMMAND_LINE_HPP */
