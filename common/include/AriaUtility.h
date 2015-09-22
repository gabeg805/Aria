/* *****************************************************************************
 * 
 * Name:    AriaUtility.h
 * Class:   <AriaUtility>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: The Aria utility tool.
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

/**
 * @brief The Aria utility tool.
 * 
 * @details Mainly used for printing usage and error state checking.
*/
namespace AriaUtility
{
    void usage(void);
    void error(std::string str);
};

#endif /* ARIA_COMMON_ARIAUTILITY_H */
