/* *****************************************************************************
 * 
 * Name:    AriaSharedMem.h
 * Class:   <AriaSharedMem>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Create and manage Aria Notification Bubble shared memory.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ARIA_COMMON_INCLUDE_ARIAMAP_H
#define ARIA_COMMON_INCLUDE_ARIAMAP_H

/* Includes */
#include <cstddef>

/* ************************************************************************** */
/**
 * @brief Generic data structure to store in the shared memory region.
 */
struct SharedMemType {
    long id;   /**< Current process PID. */
    long time; /**< Time of struct creation. */
    long x;    /**< On-screen x coordinate of the notification bubble. */
    long y;    /**< On-screen y coordinate of the notification bubble. */
    long w;    /**< Width (px) of the notification bubble. */
    long h;    /**< Height (px) of the notification bubble. */
};

/* ************************************************************************** */
/**
 * @brief Aria notification shared memory handler.
 * 
 * @details Share notification data through a memory mapped region.
*/
namespace AriaSharedMem
{
    int                    add(struct SharedMemType *data, long shift);
    int                    remove(void);
    struct SharedMemType * getfirst(void);
    struct SharedMemType * getlast(void);
    int                    memopen(void);
    int                    memwrite(struct SharedMemType *w, size_t s);
    int                    memread(struct SharedMemType *r, size_t s);
    int                    memclose(void);
    int                    memclear(void);
    int                    memmap(void);
    int                    memunmap(void);
    int                    update(void);
    int                    append(struct SharedMemType *data);
    int                    insert(struct SharedMemType *data, size_t index);
    int                    displace(struct SharedMemType *data, long shift);
    int                    find(long id);
    int                    findcurrent(void);
    int                    reorder(void);
    int                    clear(void);
    int                    clear(size_t index);
    int                    clearcurrent(void);
    size_t                 size(void);
    size_t                 length(void);
    bool                   isempty(void);
    void                   print(void);
    void                   print(size_t index);
    void                   print(struct SharedMemType *data);
};

#endif /* ARIA_COMMON_INCLUDE_ARIAMAP_H */
