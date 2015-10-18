/* *****************************************************************************
 * 
 * Name:    AriaMap.cc
 * Class:   <AriaMap>
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

/* Includes */
#include "AriaMap.h"
#include "AriaUtility.h"
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
static const  char   *MFILE   = "/tmp/ariamap";
static const  int     MPROT   = PROT_READ | PROT_WRITE;
static const  int     MFLAGS  = MAP_SHARED;
static const  size_t  MLEN    = 2*10;
static const  size_t  MSIZE   = MLEN * sizeof(struct MapData);
static        long   *MADDR   = NULL;
static const  mode_t  FMODE   = 0777;
static        int     FFLAGS  = O_RDWR;
static        int     FD      = -1;
static struct MapData MEM[MLEN];

/* ************************************************************************** */
/**
 * @brief Interface to store information in shared memory.
 * 
 * @details Executes the whole process of mapping the shared memory region,
 *          storing the data, unmapping, and cleanup.
 * 
 * @param data information to store in shared memory.
 * 
 * @param shift number of pixels to separate elements that overlap.
 */
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

/* ************************************************************************** */
/**
 * @brief Separate the new notification from other notifications by the given
 *        amount.
 * 
 * @details In the event that notifications overlap, ensure that the new
 *          notification is separated from the present notifications by the given
 *          amount.
 * 
 * @param data information to store in shared memory.
 * 
 * @param shift number of pixels to separate elements that overlap.
 */
int AriaMap::displace(struct MapData *data, long shift)
{
    // long   x      = data->x;
    long   y      = data->y;
    // long   w      = data->w;
    long   h      = data->h;
    // long   xcur   = 0;
    long   ycur   = 0;
    // long   xnew   = x;
    long   ynew   = y;
    // long   xavail = x;
    long   yavail = y;
    size_t i;
    for ( i = 0; i < MLEN; ++i ) {
        if ( MEM[i].id == 0 )
            break;

        // xcur = MEM[i].x + MEM[i].w + shift;
        ycur = MEM[i].y + MEM[i].h + shift;
        // xnew = xavail   + w        + shift;
        ynew = yavail   + h        + shift;

        if ( (ynew > MEM[i].y) && (ycur > yavail) )
            yavail = ycur;
    }

    /* Algorithm still needs to make sure that there is no overlap */
    // data->x = (xavail == x) ? (x + shift) : xavail;
    data->y = (yavail == y) ? (y + shift) : yavail;

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Cleanup shared memory that is no longer being used.
 * 
 * @details Cleanup shared memory that is no longer being used and reorder the
 *          shared memory region so that used memory is at the start.
 */
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

/* ************************************************************************** */
/**
 * @brief Open a file descriptor to the shared memory region.
 * 
 * @details Open a file descriptor to the shared memory region. When the file
 *          descriptor is determined, set a mutex on the file descriptor so that
 *          there are no collisions between other instances of Aria.
 */
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
        AriaUtility::errcheck(FD, "open", errno);
        return -1;
    }

    status = flock(FD, LOCK_EX);
    AriaUtility::errcheck(status, "flock", errno);
    return first;
}

/* ************************************************************************** */
/**
 * @brief Write data to the shared memory region.
 * 
 * @details Write data to the shared memory region, specifying how much data to
 *          write.  This write will begin at the start of the shared memory
 *          region.
 */
int AriaMap::writefd(struct MapData *w, size_t s)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return -1;

    int status;
    status = lseek(FD, 0, SEEK_SET);
    AriaUtility::errcheck(status, "lseek", errno);
    status = write(FD, w, s);
    AriaUtility::errcheck(status, "write", errno);
    status = lseek(FD, 0, SEEK_SET);
    AriaUtility::errcheck(status, "lseek", errno);
    return 0;
}

/* ************************************************************************** */
/**
 * @brief Read from the shared memory region.
 * 
 * @details Read the number of bytes, beginning from the start of the shared
 *          memory region.
 */
int AriaMap::readfd(struct MapData *r, size_t s)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return -1;

    int status;
    status = lseek(FD, 0, SEEK_SET);
    AriaUtility::errcheck(status, "lseek", errno);
    status = read(FD, r, s);
    AriaUtility::errcheck(status, "read", errno);
    status = lseek(FD, 0, SEEK_SET);
    AriaUtility::errcheck(status, "lseek", errno);
    return 0;
}

/* ************************************************************************** */
/**
 * @brief Clear the shared memory region.
 * 
 * @details Clear the shared memory region by writing 0 to the local copy and
 *          the actual shared memory region.
 */
int AriaMap::clearfd(void)
{
    clear();
    return writefd(MEM, MSIZE);
}

/* ************************************************************************** */
/**
 * @brief Map the shared memory region.
 */
int AriaMap::map(void)
{
    if ( FD < 0 )
        return -1;

    MADDR = (long *) mmap(NULL, MSIZE, MPROT, MFLAGS, FD, 0);
    if ( MADDR == MAP_FAILED ) {
        AriaUtility::errcheck(MADDR, "mmap", errno);
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Unmap the shared memory region.
 */
int AriaMap::unmap(void)
{
    if ( FD < 0 )
        return -1;

    size_t s = size();
    write(FD, MEM, s);
    int status = munmap(MADDR, MSIZE);
    AriaUtility::errcheck(status, "munmap", errno);
    close(FD);
    return 0;
}


/* ************************************************************************** */
/**
 * @brief Insert data to the local copy of the shared memory region.
 * 
 * @param data information to store in shared memory.
 */
int AriaMap::insert(struct MapData *data)
{
    int len = length();
    if ( (FD < 0) || (MADDR == NULL) || (len == MLEN) )
        return -1;

    size_t index = (len == 0) ? 0 : len-1;
    MEM[index]   = *data;
    return 0;
}


/* ************************************************************************** */
/**
 * @brief Copy data from the shared memory region to the local copy.
 * 
 * @param status a flag that indicates the status of the opened file descriptor.
 */
int AriaMap::copy(bool status)
{
    int ret = (status) ? clearfd() : 0;
    return (ret == 0) ? readfd(MEM, MSIZE) : -1;
}

/* ************************************************************************** */
/**
 * @brief Find an element in shared memory.
 * 
 * @details Searches for a match between the local copy data element's PID and
 *          the given value.
 * 
 * @param id the PID to match with the local copy data.
 */
int AriaMap::find(long id)
{
    size_t i;
    for ( i = 0; i < MLEN; ++i )
        if ( MEM[i].id == id )
            return i;
    return -1;
}

/* ************************************************************************** */
/**
 * @brief Clear data in the local copy of the shared memory region.
 */
void AriaMap::clear(void)
{
    size_t i;
    for ( i = 0; i < MLEN; ++i )
        memset(&MEM[i], 0, sizeof(*MEM));
}

/* ************************************************************************** */
/**
 * @brief Clear data in the local copy of the shared memory region.
 * 
 * @details Clears data in the local copy starting from the given start, and
 *          going the given length.
 * 
 * @param start the index to begin with.
 * 
 * @param len the number of elements to clear.
 */
void AriaMap::clear(long start, long len)
{
    size_t size = start+len;
    size_t i;
    for ( i = start; (i < size) && (i < MLEN); ++i )
        memset(&MEM[i], 0, sizeof(*MEM));
}

/* ************************************************************************** */
/**
 * @brief The size of the shared memory region.
 */
size_t AriaMap::size(void)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return 0;

    int len = length();
    return (len == 0) ? 0 : (len-1)*sizeof(*MEM);
}

/* ************************************************************************** */
/**
 * @brief The length of the shared memory region.
 * 
 * @details The number of elements (data chunks) in the shared memory region.
 */
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

/* ************************************************************************** */
/**
 * @brief Print the contents of the shared memory region.
 */
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
