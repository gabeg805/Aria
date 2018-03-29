/**
 * @file util.cpp
 * @author Gabriel Gonzalez
 * 
 * @brief Utility functions.
 */

#include "util.hpp"
#include <string>

ARIA_NAMESPACE

/**
 * @brief Replace all instances of a subtring in a string.
 * 
 * @param[in,out] text    The text to replace the substring in.
 * @param[in]     find    The string to find in the input string.
 * @param[in]     replace The string to replace the substring with.
 * 
 * @return The input string to replace. This function modifies the input string
 *         so capturing the ouput is redundant.
 */
std::string util::replace_all(std::string& text, std::string find, std::string replace)
{
    size_t length = replace.length();
    size_t i;
    for(i=0; (i=text.find(find, i)) != std::string::npos; )
    {
        text.replace(i, length, replace);
        i += length;
    }
    return text;
}

ARIA_NAMESPACE_END
