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
    // /**
    //  * Find option in option list
    //  */
    // option_t optlist_t::find(std::string opt)
    // {
    //     option_t o;
    //     for (option_t option : list) {
    //         std::cout << option.longopt << std::endl;
    //         o = option;
    //     }
    //     return o;
    // }


    interface::interface(const optlist_t options) : m_options(options)
    {
    }

    /**
     * Print program usage
     */
    void interface::usage(void)
    {
        char arg[32];
        printf("Usage: %s [option]...\n\n", PROGRAM);
        printf("Options:");
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
        char *key = *arg;
        const option_t* option;
        int listflag = 0;

        for ( ; key != NULL; ++arg, key=*arg) {
            if (!this->is_option(key)) {
                fprintf(stderr, "%s: Invalid option '%s'\n", PROGRAM, key);
                exit(1);
            }

            option = this->find_option(key);

            if (this->is_short_option(key)) {
                printf("This is a short option.\n");
            }
            else {
                printf("This is a long option.\n");
            }
            printf("~%s~\n", key);

            this->m_table[key] = "";

            switch (option->argument) {
            case commandline::no_argument:
                continue;
            case commandline::list_argument:
                listflag = 1;
                continue;
            case commandline::optional_argument:
                if (this->is_option(*(arg+1))) {
                    continue;
                }
            case commandline::required_argument:
            default:
                ++arg;
                printf("~%s~\n", *arg);
                this->m_table[key] = *arg;
                break;
            }

            // printf("~%s~\n", *a);
        }
        return;
    }

    /**
     * Return the value for the given option
     */
    std::string interface::get(std::string opt)
    {
        return this->m_table[opt];
    }

    /**
     * Check if the given option was entered
     */
    bool interface::has(std::string opt)
    {
        return (this->m_table.find(opt) != this->m_table.end());
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
                return "";
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
     * Check if the given option is a valid short or long command line option
     */
    bool interface::is_option(std::string opt)
    {
        return (this->is_short_option(opt) || this->is_long_option(opt));
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

// namespace commandline
// {
//     /**
//      * Create instance
//      */
//     parser::make_type parser::init(const options&& opts)
//     {
//         return std::make_unique<parser>(std::forward<decltype(opts)>(opts));
//     }

//     /**
//      * Construct parser
//      */
//     parser::parser(const options&& opts)
//         : m_opts(std::forward<decltype(opts)>(opts))
//     {
//     }

//     /**
//      * Print application usage message
//      */
//     void parser::usage(void) const
//     {
//         printf("Usage: %s [option]...\n\n", PROGRAM);

//         // get the length of the longest string in the flag column
//         // which is used to align the description fields
//         size_t maxlen{0};

//         for (const auto& m_opt : m_opts) {
//             size_t len{m_opt.longflag.length() + m_opt.shortflag.length() + 4};
//             maxlen = len > maxlen ? len : maxlen;
//         }

//         for (auto& opt : m_opts) {
//             size_t pad = maxlen - opt.longflag.length() - opt.token.length();

//             printf("  %s, %s", opt.shortflag.c_str(), opt.longflag.c_str());

//             if (!opt.token.empty()) {
//                 printf("=%s", opt.token.c_str());
//                 pad--;
//             }

//             // output the list with accepted values
//             if (!opt.values.empty()) {
//                 printf("%*s\n", static_cast<int>(pad + opt.desc.length()), opt.desc.c_str());

//                 pad = pad + opt.longflag.length() + opt.token.length() + 7;

//                 printf("%*c%s is one of: ", static_cast<int>(pad), ' ', opt.token.c_str());

//                 for (auto& v : opt.values) {
//                     printf("%s%s", v.c_str(), v != opt.values.back() ? ", " : "");
//                 }
//             } else {
//                 printf("%*s", static_cast<int>(pad + opt.desc.length()), opt.desc.c_str());
//             }

//             printf("\n");
//         }

//         printf("\n");
//     }

//     /**
//      * Process input values
//      */
//     void parser::process_input(const std::vector<std::string>& values)
//     {
//         bool skip = false;
//         std::string current;
//         std::string next;
//         std::string value;

//         /* */
//         for (size_t i = 0; i < values.size(); i++) {
//             current = values[i];
//             next = (values.size() > i+1) ? values[i+1] : "";

//             if (!current.compare("-h") || !current.compare("--help")) {
//                 parser::usage();
//                 exit(0);
//             }

//             if (skip) {
//                 skip = false;
//                 if (!next.empty()) {
//                     continue;
//                 }
//             }

//             /* Set the option that is found */
//             for (auto&& opt : m_opts) {
//                 if (is(current, opt.shortflag, opt.longflag)) {
//                     value = set_value(opt, current, next);
//                     skip = (value == next);
//                     break;
//                 }
//             }

//             /* Unknown option */
//             if (skip || (current[0] != '-')) {
//                 continue;
//             }
//             else {
//                 std::cout << "Unknown option " + current << std::endl;
//             }
//         }
//     }

//     /**
//      * Process configuration file values
//      */
//     void parser::process_config(void)
//     {
//         /* Fill in default values not specified */
//         std::vector<std::string> keys = aria::config::get_keys("Main");
//         std::string value;

//         for (std::string k : keys) {
//             if (has_option(k)) {
//                 continue;
//             }

//             value = aria::config::read("Main", k.c_str());
//             set_value(k, value);
//         }
//     }

//     /**
//      * Parse option value. No value is present if there is no token
//      */
//     std::string parser::parse_value(std::string current, const std::string& next,
//                                     std::string token, choices values)
//     {
//         return (token.empty()) ? "" : parse_value(current, next, values);
//     }

//     /**
//      * Parse option value
//      */
//     std::string parser::parse_value(std::string current, const std::string& next,
//                                     choices values)
//     {
//         std::string opt = std::move(current);
//         size_t pos;
//         std::string value;

//         /* Determine the value and ensure the argument is not missing */
//         if (next.empty() && opt.compare(0, 2, "--") != 0) {
//             std::cout << "Missing argument for option " + opt << std::endl;
//         }
//         else if ((pos = opt.find('=')) == std::string::npos && opt.compare(0, 2, "--") == 0) {
//             std::cout << "Missing argument for option " + opt << std::endl;
//         }
//         else if (pos == std::string::npos && !next.empty()) {
//             value = next;
//         }
//         else {
//             value = opt.substr(pos + 1);
//             opt = opt.substr(0, pos);
//         }

//         /* Invalid argument */
//         if (!values.empty() && std::find(values.begin(), values.end(), value) == values.end()) {
//             std::cout << "Invalid argument for option " + opt << std::endl;
//         }

//         return value;
//     }

//     /**
//      * Set the value defined for the given option
//      */
//     std::string parser::set_value(option opt, std::string current, std::string next)
//     {
//         auto value = parse_value(current, next, opt.token, opt.values);
//         m_optvalues.insert(std::make_pair(opt.longflag.substr(2), value));
//         return value;
//     }

//     /**
//      * Set the value defined for the given option
//      */
//     std::string parser::set_value(std::string opt, std::string value)
//     {
//         m_optvalues.insert(std::make_pair(opt, value));
//         return value;
//     }

//     /**
//      * Return the value for an option
//      */
//     std::string parser::get_value(std::string opt)
//     {
//         if (has_option(std::forward<std::string>(opt))) {
//             return m_optvalues.find(opt)->second;
//         }
//         return "";
//     }

//     /**
//      * Return all options and values
//      */
//     values parser::get_values(void)
//     {
//         return m_optvalues;
//     }

//     /**
//      * Test if the passed option was provided
//      */
//     bool parser::has_option(const std::string& option) const
//     {
//         return m_optvalues.find(option) != m_optvalues.end();
//     }

//     /**
//      * Compare option with its short version
//      */
//     bool parser::is_short(const std::string& option, const std::string& optshort) const
//     {
//         // return option.compare(optshort) == 0;
//         return option.compare(0, optshort.length(), optshort) == 0;
//     }

//     /**
//      * Compare option with its long version
//      */
//     bool parser::is_long(const std::string& option, const std::string& optlong) const
//     {
//         // return option.compare(optlong) == 0;
//         return option.compare(0, optlong.length(), optlong) == 0;
//     }

//     /**
//      * Compare option with both versions
//      */
//     bool parser::is(const std::string& option, std::string optshort,
//                     std::string optlong) const
//     {
//         return is_short(option, move(optshort)) || is_long(option, move(optlong));
//     }
// }

ARIA_NAMESPACE_END
