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
#ifndef ARIA_MAP_H
#define ARIA_MAP_H

/* Includes */
#include <cstddef>

/* Classes */
namespace AriaMap
{
    int  store(long val);
    int  displace(long h, long *y);
    int  copy(void);
    int  openfd(void);
    int  writefd(long *w, size_t s);
    int  readfd(long *r, size_t s);
    int  clearfd(void);
    int  map(void);
    int  unmap(void);
    void print(void);
    int  find(long val);
    int  length(void);
    int  clean(void);
    int  clear(void);
    int  clear(long start, long len);
};

#endif /* ARIA_MAP_H */
