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
    void error(std::string str);
    void checkery(int ret, const char *str, int err);
    void checkery(void *ptr, const char *str, int err);
};

#endif /* ARIA_COMMON_INCLUDE_ARIAUTILITY_H */
