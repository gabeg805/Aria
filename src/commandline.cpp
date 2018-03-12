/* -----------------------------------------------------------------------------
 * 
 * Name:    commandline.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None
 * 
 * Description: Stuff
 * 
 * Notes: None
 * 
 * -----------------------------------------------------------------------------
 */

/* Includes */
#include "commandline.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/* Namespace */
namespace commandline
{
    /**
     * Create instance
     */
    parser::make_type parser::make(std::string&& scriptname, const options&& opts)
    {
        return std::make_unique<parser>("Usage: " + scriptname + " [OPTION]...",
                                       std::forward<decltype(opts)>(opts));
    }

    /**
     * Construct parser
     */
    parser::parser(std::string&& synopsis, const options&& opts)
        : m_synopsis(std::forward<decltype(synopsis)>(synopsis)),
          m_opts(std::forward<decltype(opts)>(opts))
    {
    }

    /**
     * Print application usage message
     */
    void parser::usage() const
    {
        printf("%s\n\n", m_synopsis.c_str());

        // get the length of the longest string in the flag column
        // which is used to align the description fields
        size_t maxlen{0};

        for (const auto& m_opt : m_opts) {
            size_t len{m_opt.longflag.length() + m_opt.shortflag.length() + 4};
            maxlen = len > maxlen ? len : maxlen;
        }

        for (auto& opt : m_opts) {
            size_t pad = maxlen - opt.longflag.length() - opt.token.length();

            printf("  %s, %s", opt.shortflag.c_str(), opt.longflag.c_str());

            if (!opt.token.empty()) {
                printf("=%s", opt.token.c_str());
                pad--;
            }

            // output the list with accepted values
            if (!opt.values.empty()) {
                printf("%*s\n", static_cast<int>(pad + opt.desc.length()), opt.desc.c_str());

                pad = pad + opt.longflag.length() + opt.token.length() + 7;

                printf("%*c%s is one of: ", static_cast<int>(pad), ' ', opt.token.c_str());

                for (auto& v : opt.values) {
                    printf("%s%s", v.c_str(), v != opt.values.back() ? ", " : "");
                }
            } else {
                printf("%*s", static_cast<int>(pad + opt.desc.length()), opt.desc.c_str());
            }

            printf("\n");
        }

        printf("\n");
    }

    /**
     * Process input values
     */
    void parser::process_input(const std::vector<std::string>& values)
    {
        for (size_t i = 0; i < values.size(); i++) {
            std::cout << "Value (" + std::to_string(i) + "): " + values[i] << std::endl;
            parse(values[i], values.size() > i + 1 ? values[i + 1] : "");
        }
    }

    /**
     * Test if the passed option was provided
     */
    bool parser::has(const std::string& option) const
    {
        return m_optvalues.find(option) != m_optvalues.end();
    }

    /**
     * Get the value defined for given option
     */
    std::string parser::get(std::string opt) const
    {
        if (has(std::forward<std::string>(opt))) {
            return m_optvalues.find(opt)->second;
        }
        return "";
    }

    /**
     * Compare option value with given string
     */
    bool parser::compare(std::string opt, const std::string& val) const
    {
        return get(move(opt)) == val;
    }

    /**
     * Compare option with its short version
     */
    bool parser::is_short(const std::string& option, const std::string& opt_short) const
    {
        return option.compare(0, opt_short.length(), opt_short) == 0;
    }

    /**
     * Compare option with its long version
     */
    bool parser::is_long(const std::string& option, const std::string& opt_long) const
    {
        return option.compare(0, opt_long.length(), opt_long) == 0;
    }

    /**
     * Compare option with both versions
     */
    bool parser::is(const std::string& option, std::string opt_short,
                    std::string opt_long) const
    {
        return is_short(option, move(opt_short)) || is_long(option, move(opt_long));
    }

    /**
     * Parse option value
     */
    std::string parser::parse_value(std::string input, const std::string& input_next, choices values) const
    {
        std::string opt = move(input);
        size_t pos;
        std::string value;

        if (input_next.empty() && opt.compare(0, 2, "--") != 0) {
            std::cout << "Missing argument for option " + opt << std::endl;
        } else if ((pos = opt.find('=')) == std::string::npos && opt.compare(0, 2, "--") == 0) {
            std::cout << "Missing argument for option " + opt << std::endl;
        } else if (pos == std::string::npos && !input_next.empty()) {
            value = input_next;
        } else {
            value = opt.substr(pos + 1);
            opt = opt.substr(0, pos);
        }

        if (!values.empty() && std::find(values.begin(), values.end(), value) == values.end()) {
            std::cout << "Invalid argument for option " + opt << std::endl;
        }

        return value;
    }

    /**
     * Parse and validate passed arguments and flags
     */
    void parser::parse(const std::string& input, const std::string& input_next) {
        auto skipped = m_skipnext;
        if (m_skipnext) {
            m_skipnext = false;
            if (!input_next.empty()) {
                return;
            }
        }

        for (auto&& opt : m_opts) {
            if (is(input, opt.shortflag, opt.longflag)) {
                if (opt.token.empty()) {
                    m_optvalues.insert(make_pair(opt.longflag.substr(2), ""));
                } else {
                    auto value = parse_value(input, input_next, opt.values);
                    m_skipnext = (value == input_next);
                    m_optvalues.insert(make_pair(opt.longflag.substr(2), value));
                }
                return;
            }
        }

        if (skipped) {
            return;
        } else if (input[0] != '-') {
            m_posargs.emplace_back(input);
        } else {
            std::cout << "Unrecognized option " + input << std::endl;
        }
    }
}
