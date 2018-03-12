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
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace commandline
{

    class option;
    using choices = std::vector<std::string>;
    using options = std::vector<option>;
    using values = std::map<std::string, std::string>;
    using posargs = std::vector<std::string>;

    class option
    {
    public:
        std::string shortflag;
        std::string longflag;
        std::string desc;
        std::string token;
        const choices values;

        explicit option(std::string&& shortflag, std::string&& longflag,
                        std::string&& desc, std::string&& token = "",
                        const choices&& c = {})
            : shortflag(std::forward<std::string>(shortflag))
            , longflag(std::forward<std::string>(longflag))
            , desc(std::forward<std::string>(desc))
            , token(std::forward<std::string>(token))
            , values(std::forward<const choices>(c)) {}
    };

    class parser
    {
    public:
        using make_type = std::unique_ptr<parser>;
        static make_type make(std::string&& scriptname, const options&& opts);

        explicit parser(std::string&& synopsis, const options&& opts);

        void usage() const;

        void process_input(const std::vector<std::string>& values);

        bool has(const std::string& option) const;
        std::string get(std::string opt) const;
        bool compare(std::string opt, const std::string& val) const;

    protected:
        bool is_short(const std::string& option, const std::string& opt_short) const;
        bool is_long(const std::string& option, const std::string& opt_long) const;
        bool is(const std::string& option, std::string opt_short, std::string opt_long) const;

        std::string parse_value(std::string input, const std::string& input_next, choices values) const;
        void parse(const std::string& input, const std::string& input_next = "");

    private:
        std::string m_synopsis{};
        const options m_opts;
        values m_optvalues{};
        posargs m_posargs{};
        bool m_skipnext{false};
    };

}
