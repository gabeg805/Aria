/**
 * @file commandline.cpp
 * @author Gabriel Gonzalez
 * 
 * @note Check if when checking for long option and finding long option, it
 *       compares only the stuff before the equals.
 * 
 * @brief A command line interface utility to parse options, print usage, and
 *        notify the user when an error occurs.
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
     */
    interface::interface(const optlist_t options) : m_options(options)
    {
    }

    /**
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
     * @details Iterate over the argument list. Check if the current arg has a
     *          list_argument type, and if it is, store each argument.
     *          Otherwise, the current arg either has a type which takes 0 or 1
     *          argument. Store the argument, if present, and increment the
     *          argument list vector by the proper amount.
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
     * @details Used as a test to make sure that command line options were
     *          interpretted correctly. If there is ever any doubt, this
     *          function can be used.
     */
    void interface::test(void)
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
     */
    std::string interface::get(std::string opt)
    {
        return (this->has(opt)) ? this->m_table[this->to_key(opt)].at(0) : "";
    }

    /**
     */
    bool interface::has(std::string opt)
    {
        return (this->m_table.find(this->to_key(opt)) != this->m_table.end());
    }

    /**
     * @details The most complicated sections in this are the optional and
     *          required argument type sections.
     * 
     *          If an optional_argument type, check if the current option is a
     *          short option, and if the next string in the argument pointer is
     *          an option. You check for a short option because a long option
     *          typically has an argument, and you check the next string in the
     *          argument pointer because if the next string is an option, in
     *          conjunction with a short option, then there is no argument for
     *          this option.
     * 
     *          If a required_argument type, if it's a short option, then the
     *          next string in the argument pointer is the value. Otherwise,
     *          it's a long option, so I need to extract the option and value
     *          (from the form '--long-option=value').
     * 
     * @todo Test option_argument for a long option with no argument.
     * @todo Test optional and list argument type.  It looks like the list argument
     *       can act as an optional argument as well.
     */
    char** interface::parse_argument(const option_t* option, char** arg,
                                     bool& listflag)
    {
        std::string key = *arg;
        std::string value;
        if (!option)
        {
            return NULL;
        }

        switch (option->argument)
        {
        case commandline::no_argument:
            if (option->longopt == "--help")
            {
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
            if (this->is_short_option(option, key))
            {
                ++arg;
                value = *arg;
            }
            else
            {
                value = this->extract_value(key);
                key = this->extract_option(key);
            }
            this->set(key, value);
            break;
        }

        return arg;
    }

    /**
     * @details This function is meant to be called multiple times in different
     *          points of the argument list pointer, so as to capture all
     *          arguments of a list_argument type option.
     */
    bool interface::parse_list_argument(char** arg, std::string key, bool& listflag)
    {
        if (listflag)
        {
            if (this->is_option(*arg))
            {
                listflag = false;
            }
            else
            {
                this->set(key, *arg);
            }
        }
        return listflag;
    }

    /**
     */
    const option_t* interface::find_option(std::string opt)
    {
        optlist_t::const_iterator it;
        const option_t* ptr;
        for (it=this->m_options.cbegin(); it != this->m_options.cend(); ++it)
        {
            ptr = &(*it);
            if (this->is_option(ptr, opt))
            {
                return ptr;
            }
        }
        return NULL;
    }

    /**
     */
    std::string interface::extract(std::string opt, int field)
    {
        if ((field != 1) && (field != 2))
        {
            return "";
        }

        int length = opt.length();
        int i;
        for (i=0; i < length; ++i)
        {
            if (opt[i] == '=')
            {
                break;
            }
            if ((i+1) == length)
            {
                return opt;
            }
        }

        if (field == 1)
        {
            return opt.substr(0, i);
        }
        ++i;
        return opt.substr(i, length-i);
    }

    /**
     */
    std::string interface::extract_option(std::string opt)
    {
        return this->extract(opt, 1);
    }

    /**
     */
    std::string interface::extract_value(std::string opt)
    {
        return this->extract(opt, 2);
    }

    /**
     */
    std::string interface::to_short_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        return (option) ? option->shortopt : "";
    }

    /**
     */
    std::string interface::to_long_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        return (option) ? option->longopt : "";
    }

    /**
     * @details Check if the input string has any dashes in front. If not, try
     *          long option dashes first, and if that doesn't work, resort to
     *          the short option dash. Find the corresponding option struct,
     *          strip the leading dash(es) and return the key.
     * 
     *          The key will be used in m_table, and will have a corresponding
     *          value pair. By default, the long option is used as the key,
     *          without the leading dashes. However, if there is no long option,
     *          the short option is used, also without the leading dash.
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
     */
    bool interface::is_option(std::string opt)
    {
        return (this->is_short_option(opt) || this->is_long_option(opt));
    }

    /**
     */
    bool interface::is_option(const option_t* option, std::string opt)
    {
        return (this->is_short_option(option, opt) || this->is_long_option(option, opt));
    }

    /**
     */
    bool interface::is_short_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        return this->is_short_option(option, opt);
    }

    /**
     */
    bool interface::is_short_option(const option_t* option, std::string opt)
    {
        return (option && (opt == option->shortopt));
    }

    /**
     */
    bool interface::is_long_option(std::string opt)
    {
        const option_t* option = this->find_option(opt);
        return this->is_long_option(option, opt);
    }

    /**
     */
    bool interface::is_long_option(const option_t* option, std::string opt)
    {
        return (option && (this->extract_option(opt) == option->longopt));
    }
}
