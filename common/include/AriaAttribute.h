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
#ifndef ARIA_COMMON_ARIAATTRIBUTE_H
#define ARIA_COMMON_ARIAATTRIBUTE_H

/* Includes */
#include <string>

/* Classes */
namespace AriaAttribute
{
    int         init(char **argv);
    int         set(std::string key, std::string val);
    int         set(size_t      key, std::string val);
    int         set_defaults(void);
    std::string get(std::string key);
    std::string get(size_t      key);
    void        print(void);
};

#endif /* ARIA_COMMON_ARIAATTRIBUTE_H */
