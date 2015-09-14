/* *****************************************************************************
 * 
 * Name:    AriaMap.cc
 * Class:   <AriaMap>
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Create and manage a shared memory region for interprocess
 *              communication.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "AriaMap.h"
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <stdio.h>
#include <sys/file.h>

/* Declares */
static const  char   *MFILE   = "ploobmap";
static const  int     MPROT   = PROT_READ | PROT_WRITE;
static const  int     MFLAGS  = MAP_SHARED;
static const  size_t  MLEN    = 2*10;
static const  size_t  MSIZE   = MLEN * sizeof(struct MapData);
static        long   *MADDR   = NULL;
static const  mode_t  FMODE   = 0777;
static        int     FFLAGS  = O_RDWR;
static        int     FD      = -1;
static struct MapData MEM[MLEN];

void checkery(int ret, const char *str, int err)
{
    if ( ret < 0 )
        std::cout
            << "aria: "
            << str
            << ": "
            << std::strerror(errno)
            << std::endl;
}

void checkery(void *ptr, const char *str, int err)
{
    if ( ptr == NULL )
        std::cout
            << "aria: "
            << str
            << ": "
            << std::strerror(errno)
            << std::endl;
}

/* **********************************
 * ***** DATA SHARING INTERFACE *****
 * **********************************
 */

/* Store data in mapped memory */
int AriaMap::store(struct MapData *data, long shift)
{
    int status;
    int ret;
    if ( (status=AriaMap::openfd()) < 0 )
        return status;
    if ( (ret=AriaMap::map()) < 0 )
        return ret;
    if ( (ret=AriaMap::copy(status)) < 0 )
        return ret;
    if ( (ret=AriaMap::displace(data, shift)) < 0 )
        return ret;
    if ( (ret=AriaMap::insert(data)) < 0 )
        return ret;
    if ( (ret=AriaMap::unmap()) < 0 )
        return ret;
    return 0;
}

/* Shift y coordinate of notification bubble location */
int AriaMap::displace(struct MapData *data, long shift)
{
    long   x      = data->x;
    long   y      = data->y;
    long   w      = data->w;
    long   h      = data->h;
    long   xcur   = 0;
    long   ycur   = 0;
    long   xnew   = x;
    long   ynew   = y;
    long   xavail = x;
    long   yavail = y;
    size_t i;
    for ( i = 0; i < MLEN; ++i ) {
        if ( MEM[i].id == 0 )
            break;

        xcur = MEM[i].x + MEM[i].w + shift;
        ycur = MEM[i].y + MEM[i].h + shift;
        xnew = xavail   + w        + shift;
        ynew = yavail   + h        + shift;

        if ( (ynew > MEM[i].y) && (ycur > yavail) ) {
            yavail = ycur;
            // if ( (xnew > MEM[i].x) && (xcur > xavail) ) 
            //     xavail = xcur;
        }

        // std::cout
        //     << "i: " << i << " | "
        //     << "memy: " << MEM[i].y << " | "
        //     << "memh: " << MEM[i].h << " | "
        //     << "yavail: " << yavail
        //     << std::endl;
    }

    /* This shifts the x over like crazy, still 
     * needs to make sure that there is no overlap
     * The algorithm doesn't work as expected */
    // data->x = (xavail == x) ? (x + shift) : xavail;
    data->y = (yavail == y) ? (y + shift) : yavail;

    return 0;
}

/* Clean up used mapped memory */
int AriaMap::cleanup(void)
{
    std::cout << "Cleaning..." << std::endl;

    openfd();
    map();
    clear();
    readfd(MEM, MSIZE);
    // print();
    pid_t pid   = getpid();
    long  start = find(pid);
    clear(start, 1);

    size_t i;
    size_t j = 0;
    for ( i = start+1; i < MLEN; ++i ) {
        if ( MEM[i].id == 0 ) {
            std::cout << "break" << std::endl;
            std::cout << "start: " << start << std::endl;
            break;
        }

        MEM[start+j] = MEM[i];
        memset(&MEM[i], 0, sizeof(*MEM));
        ++j;
    }

    writefd(MEM, MSIZE);
    // print();
    unmap();

    return 0;
}

/* **********************************
 * ***** SHARED FILE DESCRIPTOR *****
 * **********************************
 */

/* Set memory mapped file descriptor */
int AriaMap::openfd(void)
{
    bool first = false;
    int status = access(MFILE, F_OK);
    if ( status < 0 ) {
        FFLAGS |= O_CREAT;
        first   = true;
    }

    FD = open(MFILE, FFLAGS, FMODE);
    if ( FD < 0 ) {
        checkery(FD, "open", errno);
        return -1;
    }

    status = flock(FD, LOCK_EX);
    checkery(status, "flock", errno);
    return first;
}

/* Write to the shared memory region */
int AriaMap::writefd(struct MapData *w, size_t s)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return -1;

    int status;
    status = lseek(FD, 0, SEEK_SET);
    checkery(status, "lseek", errno);
    status = write(FD, w, s);
    checkery(status, "write", errno);
    status = lseek(FD, 0, SEEK_SET);
    checkery(status, "lseek", errno);
    return 0;
}

/* Read memory mapped region */
int AriaMap::readfd(struct MapData *r, size_t s)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return -1;

    int status;
    status = lseek(FD, 0, SEEK_SET);
    checkery(status, "lseek", errno);
    status = read(FD, r, s);
    checkery(status, "read", errno);
    status = lseek(FD, 0, SEEK_SET);
    checkery(status, "lseek", errno);
    return 0;
}

/* Clear the shared memory region */
int AriaMap::clearfd(void)
{
    clear();
    return writefd(MEM, MSIZE);
}

/* *************************
 * ***** MAPPED MEMORY *****
 * *************************
 */

/* Setup shared memory mapped region */
int AriaMap::map(void)
{
    if ( FD < 0 )
        return -1;

    MADDR = (long *) mmap(NULL, MSIZE, MPROT, MFLAGS, FD, 0);
    if ( MADDR == MAP_FAILED ) {
        checkery(MADDR, "mmap", errno);
        return -1;
    }

    return 0;
}

/* Unmap shared memory region */
int AriaMap::unmap(void)
{
    if ( FD < 0 )
        return -1;

    size_t s = size();
    write(FD, MEM, s);
    int status = munmap(MADDR, MSIZE);
    checkery(status, "munmap", errno);
    close(FD);
    return 0;
}


/* ****************************************
 * ***** DATA STRUCTURE FUNCTIONALITY *****
 * ****************************************
 */

/* Insert data into the local copy of the mapped memory */
int AriaMap::insert(struct MapData *data)
{
    int len = length();
    if ( (FD < 0) || (MADDR == NULL) || (len == MLEN) )
        return -1;

    size_t index = (len == 0) ? 0 : len-1;
    MEM[index]   = *data;
    return 0;
}


/* Copy mapped memory to local copy */
int AriaMap::copy(bool status)
{
    int ret = (status) ? clearfd() : 0;
    return (ret == 0) ? readfd(MEM, MSIZE) : -1;
}

/* Find data in local mapped memory copy */
int AriaMap::find(long val)
{
    size_t i;
    for ( i = 0; i < MLEN; ++i )
        if ( MEM[i].id == val )
            return i;
    return -1;
}

/* Clear local mapped memory copy */
void AriaMap::clear(void)
{
    size_t i;
    for ( i = 0; i < MLEN; ++i )
        memset(&MEM[i], 0, sizeof(*MEM));
}

void AriaMap::clear(long start, long len)
{
    size_t size = start+len;
    size_t i;
    for ( i = start; (i < size) && (i < MLEN); ++i )
        memset(&MEM[i], 0, sizeof(*MEM));
}

/* Size of the local mapped memory copy */
size_t AriaMap::size(void)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return 0;

    int len = length();
    return (len == 0) ? 0 : (len-1)*sizeof(*MEM);
}

/* Length of the local mapped memory copy */
size_t AriaMap::length(void)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return 0;

    size_t i;
    for ( i = 0; i < MLEN; ++i )
        if ( MEM[i].id == 0 )
            return i+1;
    return i;
}

/* *********************
 * ***** UTILITIES *****
 * *********************
 */

/* Print memory mapped data */ 
void AriaMap::print(void)
{
    size_t i;
    for ( i = 0; i < MLEN; ++i )
        std::cout
            << "i: " << i
            << " | MEM[i]: "
            << MEM[i].id
            << " "
            << MEM[i].x
            << " "
            << MEM[i].y
            << " "
            << MEM[i].w
            << " "
            << MEM[i].h
            << std::endl;
}
