/* *****************************************************************************
 * 
 * Name:    AriaUtility.h
 * Class:   <AriaUtility>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Utility functions for various Aria scenarios.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ARIA_COMMON_ARIAUTILITY_H
#define ARIA_COMMON_ARIAUTILITY_H

/* Includes */
#include <string>

/* Classes */
namespace AriaUtility
{
    void usage(void);
    void error(std::string str);
};

#endif /* ARIA_COMMON_ARIAUTILITY_H */
