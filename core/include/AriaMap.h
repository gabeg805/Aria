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

struct MapData {
    long id;
    long x;
    long y;
    long w;
    long h;
};

/* Classes */
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
    int    find(long val);
    void   clear(void);
    void   clear(long start, long len);
    size_t size(void);
    size_t length(void);

    void   print(void);
};

#endif /* ARIA_MAP_H */
