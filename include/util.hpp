/**
 * @file util.cpp
 * @author Gabriel Gonzalez
 * 
 * @brief Utility functions.
 */

#include "aria.hpp"
#include <string>

ARIA_NAMESPACE

/**
 * @namespace util
 * 
 * @brief Utility functions.
 */
namespace util
{
    /**
     * @brief Replace all instances of a subtring in a string.
     * 
     * @param[in,out] text    The text to replace the substring in.
     * @param[in]     find    The string to find in the input string.
     * @param[in]     replace The string to replace the substring with.
     */
    std::string replace_all(std::string& text, std::string find, std::string replace);
};

ARIA_NAMESPACE_END
