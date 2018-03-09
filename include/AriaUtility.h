/* *****************************************************************************
 * 
 * Name:    AriaUtility.h
 * Class:   <AriaUtility>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Aria Notification Bubble utilities.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ARIA_COMMON_INCLUDE_ARIAUTILITY_H
#define ARIA_COMMON_INCLUDE_ARIAUTILITY_H

/* Includes */
#include <string>

/**
 * @brief The Aria utility tool.
 * 
 * @details Mainly used for printing usage and error state checking.
*/
namespace AriaUtility
{
    void usage(void);
    void errprint(std::string str);
    void errprint(std::string str, int err);
};

#endif /* ARIA_COMMON_INCLUDE_ARIAUTILITY_H */
