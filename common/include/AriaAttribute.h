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
#ifndef ARIA_ATTRIBUTE_H
#define ARIA_ATTRIBUTE_H

/* Includes */
#include <string>

/* Classes */
namespace AriaAttribute
{
    std::string get(std::string key);
    std::string get(size_t      key);
    int         set(std::string key, std::string val);
    int         set(size_t      key, std::string val);
    int         set_defaults(void);
    void        print(void);
};

#endif /* ARIA_ATTRIBUTE_H */
