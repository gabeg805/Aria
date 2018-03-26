/**
 * @file commandline.cpp
 * @author Gabriel Gonzalez
 * 
 * @brief A command line interface utility to parse options, print usage, and
 *        notify the user when an error occurs.
 * 
 * @note This utility expects "PROGRAM" to be #define-d, so that printing the
 *       program usage and displaying any errors works as expected.
 */

#include "commandline.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstdio>

namespace commandline
{
    /**
     * @brief Construct the command line interface.
     * 
     * @details Initialize the member variable m_options with the input
     *          parameter.
     * 
     * @param[in] options List of all command line options for the program.
     */
    interface::interface(const optlist_t options) : m_options(options)
    {
    }

    /**
     * @brief Print the program usage message.
     */
    void interface::usage(void)
    {
        printf("Usage: %s [option]...\n\n", PROGRAM);
        printf("Options:");
        char arg[commandline::kArgumentNameLength];

        for (option_t opt : this->m_options)
        {
            if (opt.name.empty())
            {
                arg[0] = '\0';
            }
            else
            {
                snprintf(arg, sizeof(arg), "=<%s>", opt.name.c_str());
            }
            printf("\n    %s, %s%s\n", opt.shortopt.c_str(),
                   opt.longopt.c_str(), arg);
            printf("        %s\n", opt.desc.c_str());
        }
    }

    /**
     * @brief Parse the list of arguments given on the command line.
     * 
     * @details Iterate over the argument list. Check if the current arg has a
     *          list_argument type, and if it is, store each argument.
     *          Otherwise, the current arg either has a type which takes 0 or 1
     *          argument. Store the argument, if present, and increment the
     *          argument list vector by the proper amount.
     * 
     * @param[in] argv List of command line arguments (from main function).
     */
    void interface::parse(char** argv)
    {
        char**          arg      = argv+1;
        std::string     key      = *arg;
        bool            listflag = false;
        const option_t* option;

        for ( ; *arg != NULL; ++arg)
        {
            if (this->parse_list_argument(arg, key, listflag))
            {
                continue;
            }
            key = *arg;
            if (!(option=this->find_option(key)))
            {
                fprintf(stderr, "%s: Invalid option '%s'\n", PROGRAM,
                        key.c_str());
                exit(1);
            }
            arg = this->parse_argument(option, arg, listflag);
        }
    }

    /**
     * @brief Print the command line options that have been entered.
     */
    void interface::print(void)
    {
        int i;
        for (auto it : this->m_table)
        {
            printf("%s: ", it.first.c_str());
            if (it.second.empty())
            {
                printf("\n");
                continue;
            }
            i = 0;
            for (auto a : it.second)
            {
                if (i > 0)
                {
                    printf(", ");
                }
                printf("%s", a.c_str());
                ++i;
            }
            printf("\n");
        }
    }

    /**
     * @brief Set the value for the given option.
     * 
     * @param[in] opt   An option entered in the command line.
     * @param[in] value The value to set for the given option.
     * 
     * @return 0 on success, -1 on error.
     */
    int interface::set(std::string opt, std::string value)
    {
        std::string key = this->to_key(opt);
        if (key.empty())
        {
            return -1;
        }
        this->m_table[key].push_back(value);
        return 0;
    }

    /**
     * @brief Retrieve the value for the given option.
     * 
     * @details Check if the input option has been specified on the command
     *          line, and if so, return the value. Otherwise, return an empty string.
     * 
     * @param[in] opt An option entered in the command line.
     */
    std::string interface::get(std::string opt)
    {
        return (this->has(opt)) ? this->m_table[this->to_key(opt)].at(0) : "";
    }

    /**
     * @brief Check if the given option was entered on the command line.
     * 
     * @param[in] opt An option entered in the command line.
     */
    bool interface::has(std::string opt)
    {
        return (this->m_table.find(this->to_key(opt)) != this->m_table.end());
    }

    /**
     * Parse argument and populate the value
     * To-do: Test optional and list argument
     */
    char** interface::parse_argument(const option_t* option, char** arg, bool& listflag)
    {
        std::string key = *arg;
        std::string value;

        if (!option) {
            return NULL;
        }

        switch (option->argument) {
        case commandline::no_argument:
            if (option->longopt == "--help") {
                this->usage();
                exit(0);
            }
            break;
        case commandline::list_argument:
            listflag = true;
            break;
        case commandline::optional_argument:
            if (this->is_short_option(option, key)
                && this->is_option(*(arg+1)))
            {
                this->set(key, "");
                break;
            }
        case commandline::required_argument:
        default:
            if (this->is_short_option(option, key)) {
                ++arg;
                value = *arg;
            }
            else {
                value = this->extract_value(key);
                key = this->extract_option(key);
            }
            this->set(key, value);
            break;
        }

        return arg;
    }

    /**
     * Parse list argument and populate all its values
     */
    bool interface::parse_list_argument(char** arg, std::string key, bool& listflag)
    {
        if (listflag) {
            if (this->is_option(*arg)) {
                listflag = false;
            }
            else {
                this->set(key, *arg);
            }
        }
        return listflag;
    }

    /**
     * Find option in valid command line options
     */
    const option_t* interface::find_option(std::string opt)
    {
        optlist_t::const_iterator it;
        const option_t* ptr;
        for (it=this->m_options.cbegin(); it != this->m_options.cend(); ++it)
        {
            ptr = &(*it);
            if (this->is_short_option(ptr, opt) || this->is_long_option(ptr, opt)) {
                return ptr;
            }
        }
        return NULL;
    }

    /**
     * Extract field from long option
     */
    std::string interface::extract(std::string opt, int field)
    {
        if ((field != 1) && (field != 2)) {
            return "";
        }

        int length = opt.length();
        int i;
        for (i=0; i < length; ++i) {
            if (opt[i] == '=') {
                break;
            }
            if ((i+1) == length) {
                return opt;
            }
        }

        if (field == 1) {
            return opt.substr(0, i);
        }
        ++i;
        return opt.substr(i, length-i);
    }

    /**
     * Extract option field from long option
     */
    std::string interface::extract_option(std::string opt)
    {
        return this->extract(opt, 1);
    }

    /**
     * Extract value field from long option
     */
    std::string interface::extract_value(std::string opt)
    {
        return this->extract(opt, 2);
    }

    /**
     * Convert long option to short option
     */
    std::string interface::to_short_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        if (!option) {
            return "";
        }
        return option->shortopt;
    }

    /**
     * Convert short option to long option
     */
    std::string interface::to_long_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        if (!option) {
            return "";
        }
        return option->longopt;
    }

    /**
     * @brief Convert input option to a key string.
     * 
     * Check if the input string has any dashes in front. If not, try long
     * option dashes first, and if that doesn't work, resort to the short
     * option dash. Find the corresponding option struct, strip the leading
     * dash(es) and return the key.
     * 
     * The key will be used in m_table, and will have a corresponding value
     * pair. By default, the long option is used as the key, without the leading
     * dashes. However, if there is no long option, the short option is used,
     * also without the leading dash.
     */
    std::string interface::to_key(std::string input)
    {
        const option_t* option = NULL;
        std::string opt;
        if (input[0] != '-')
        {
            input.insert(0, "--");
            if (!(option=this->find_option(input)))
            {
                input.erase(0, 1);
            }
        }
        if (!option && !(option=this->find_option(input)))
        {
            return "";
        }
        if (!option->longopt.empty())
        {
            opt = option->longopt.substr(2);
        }
        else if (!option->shortopt.empty())
        {
            opt = option->shortopt.substr(1);
        }
        else
        {
            return "";
        }
        return opt;
    }

    /**
     * Check if the given option is a valid short or long command line option
     */
    bool interface::is_option(std::string opt)
    {
        return (this->is_short_option(opt) || this->is_long_option(opt));
    }

    /**
     * Check if the given option is a valid short or long command line option
     */
    bool interface::is_option(const option_t* option, std::string opt)
    {
        return (this->is_short_option(option, opt) || this->is_long_option(option, opt));
    }

    /**
     * Check if the given option is a valid short command line option
     */
    bool interface::is_short_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        return this->is_short_option(option, opt);
    }

    /**
     * Check if the given option is a valid short command line option
     */
    bool interface::is_short_option(const option_t* option, std::string opt)
    {
        return (option && (opt == option->shortopt));
    }

    /**
     * Check if the given option is a valid long command line option
     */
    bool interface::is_long_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        return this->is_long_option(option, opt);
    }

    /**
     * Check if the given option is a valid long command line option
     */
    bool interface::is_long_option(const option_t* option, std::string opt)
    {
        return (option && (this->extract_option(opt) == option->longopt));
    }

}
