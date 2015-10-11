/* *****************************************************************************
 * 
 * Name:    AriaAttribute.h
 * Class:   <AriaAttribute>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Manage Aria Notification Bubble attributes.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

// Header guard
#ifndef ARIA_COMMON_INCLUDE_ARIAATTRIBUTE_H
#define ARIA_COMMON_INCLUDE_ARIAATTRIBUTE_H

/* Includes */
#include <string>

/**
 * @brief Aria notification attribute handler.
 * 
 * @details Stores the data structure that contains all attributes for the Aria
 *          notification bubble. Is capable of searching, accessing, and 
 *          modifying elements in the attribute data structure and returning the
 *          appropriate value back to the user.
*/
namespace AriaAttribute
{
    int         init(char **argv);
    int         setstr(std::string key, std::string val);
    int         setint(std::string key, int val);
    int         setdef(void);
    std::string getstr(std::string key);
    int         getint(std::string key);
    void        print(void);
};

#endif /* ARIA_COMMON_INCLUDE_ARIAATTRIBUTE_H */
