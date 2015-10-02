/* *****************************************************************************
 * 
 * Name:    AriaMap.h
 * Class:   <AriaMap>
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

/* Header guard */
#ifndef ARIA_COMMON_ARIAMAP_H
#define ARIA_COMMON_ARIAMAP_H

/* Includes */
#include <cstddef>

/* ************************************************************************** */
/**
 * @brief Generic data structure to store in the shared memory region.
 */
struct MapData {
    long id; /**< Current process PID. */
    long x;  /**< On-screen x coordinate of the notification bubble. */
    long y;  /**< On-screen y coordinate of the notification bubble. */
    long w;  /**< Width (px) of the notification bubble. */
    long h;  /**< Height (px) of the notification bubble. */
};

/* ************************************************************************** */
/**
 * @brief Aria notification shared memory handler.
 * 
 * @details Share notification data through a memory mapped region.
*/
namespace AriaMap
{
    int    store(struct MapData *data, long shift);
    int    displace(struct MapData *data, long shift);
    int    cleanup(void);

    int    openfd(void);
    int    writefd(struct MapData *w, size_t s);
    int    readfd(struct MapData *r, size_t s);
    int    clearfd(void);

    int    map(void);
    int    unmap(void);

    int    insert(struct MapData *data);
    int    copy(bool status);
    int    find(long id);
    void   clear(void);
    void   clear(long start, long len);
    size_t size(void);
    size_t length(void);

    void   print(void);
};

#endif /* ARIA_COMMON_ARIAMAP_H */
