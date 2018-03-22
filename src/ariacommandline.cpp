/* -----------------------------------------------------------------------------
 * 
 * Name:    ariacommandline.h
 * Class:   <commandline>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None
 * 
 * Description: Command line parser.
 * 
 * Notes: None
 * 
 * -----------------------------------------------------------------------------
 */

/* Includes */
#include "ariacommandline.h"
#include "ariadef.h"
#include "ariaconf.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstdio>

ARIA_NAMESPACE

namespace commandline
{
    interface::interface(const optlist_t options) : m_options(options)
    {
    }

    /**
     * Print program usage
     */
    void interface::usage(void)
    {
        printf("Usage: %s [option]...\n\n", PROGRAM);
        printf("Options:");
        char arg[32];

        for (option_t opt : this->m_options) {
            if (opt.name.empty()) {
                arg[0] = '\0';
            }
            else {
                snprintf(arg, sizeof(arg), "=<%s>", opt.name.c_str());
            }
            printf("\n    %s, %s%s\n", opt.shortopt.c_str(),
                   opt.longopt.c_str(), arg);
            printf("        %s\n", opt.desc.c_str());
        }
        return;
    }

    /**
     * Parse command line options
     */
    void interface::parse(char** argv)
    {
        char** arg = argv+1;
        std::string key = *arg;
        const option_t* option;
        bool listflag = false;

        for ( ; *arg != NULL; ++arg) {
            if (this->parse_list_argument(arg, key, listflag)) {
                continue;
            }

            key = *arg;
            option = this->find_option(key);
            if (!option) {
                fprintf(stderr, "%s: Invalid option '%s'\n", PROGRAM, key.c_str());
                exit(1);
            }

            arg = this->parse_argument(option, arg, listflag);
        }
    }

    /**
     * Print given command line options
     */
    void interface::print(void)
    {
        int i;
        for (auto it : this->m_table) {
            printf("%s: ", it.first.c_str());
            if (it.second.empty()) {
                printf("\n");
                continue;
            }
            i = 0;
            for (auto a : it.second) {
                if (i > 0) {
                    printf(", ");
                }
                printf("%s", a.c_str());
                ++i;
            }
            printf("\n");
        }
    }
    /**
     * Set the value for the given option
     */
    void interface::set(std::string key, std::string value)
    {
        this->m_table[this->to_long_option(key)].push_back(value);
    }

    /**
     * Return the value for the given option
     */
    std::string interface::get(std::string opt)
    {
        return (this->has(opt)) ? this->m_table[this->to_long_option(opt)].at(0) : "";
    }

    /**
     * Check if the given option was entered
     */
    bool interface::has(std::string opt)
    {
        return (this->m_table.find(this->to_long_option(opt)) != this->m_table.end());
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

ARIA_NAMESPACE_END
