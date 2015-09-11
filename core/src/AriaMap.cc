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

/* CAN ALSO USE FLOCK FOR LOCKING */

/* Declares */
static const size_t max_len    = 1 + 2*10; /* change file size according to data */

static const char  *MFILE   = "ploobmap";
static const int    MPROT   = PROT_READ | PROT_WRITE;
static const int    MFLAGS  = MAP_SHARED; /* Map anonymous flag */
static const size_t MSIZE   = sizeof(long) * max_len;

static       int    FFLAGS = O_RDWR; // | O_APPEND;
static const mode_t FMODE   = 0777;

static       int    fd     = -1;
static       long  *addr   = NULL;
static       bool   is_first   = false;
static       long   mem[max_len];
static       long   space      = 10;
// static       size_t start   = 0;
static       size_t counter = 0;

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

/* Store data in the local copy of the mapped memory */
int AriaMap::store(long val)
{
    if ( (fd < 0) || (addr == NULL) || (length() == max_len) )
        return -1;

    int    len   = length();
    size_t index = (len == 0) ? 0 : len-1;
    mem[index] = val;
    ++counter;

    return 0;
}

/* Shift y coordinate of notification bubble location */
int AriaMap::displace(long h, long *y)
{
    long   pos  = *y;
    long   orig = pos;
    long   size;
    size_t i;
    for ( i = 0; i < max_len; i+=3 ) {
        if ( mem[i] == -1 )
            break;

        size = mem[i+1] + mem[i+2];
        if ( size >= pos )
            pos = size;
    }

    *y = (pos != orig) ? (pos + space) : orig;
    return 0;
}

/* Copy mapped memory to local data copy */
int AriaMap::copy(void)
{
    if ( is_first ) {
        clearfd();
        is_first = false;
    }

    readfd(mem, MSIZE);

    return 0;
}

/* **********************************
 * ***** SHARED FILE DESCRIPTOR *****
 * **********************************
 */

/* Set memory mapped file descriptor */
int AriaMap::openfd(void)
{
    /* Check existence of shared memory file */
    int status = access(MFILE, F_OK);
    if ( status < 0 ) {
        FFLAGS  |= O_CREAT;
        is_first = true;
        /* std::cout
            << "aria: Map file access: "
            << std::strerror(errno)
            << std::endl; */
    }

    /* Open shared memory file */
    fd = open(MFILE, FFLAGS, FMODE);
    if ( fd < 0 ) {
        checkery(fd, "open", errno);
        return -1;
    }

    return (int) is_first;
}

/* Write to the shared memory region */
int AriaMap::writefd(long *w, size_t s)
{
    if ( (fd < 0) || (addr == NULL) )
        return -1;

    int status;
    status = lseek(fd, 0, SEEK_SET);
    checkery(status, "lseek", errno);
    status = write(fd, w, s);
    checkery(status, "write", errno);
    status = lseek(fd, 0, SEEK_SET);
    checkery(status, "lseek", errno);

    return 0;
}

/* Read memory mapped region */
int AriaMap::readfd(long *r, size_t s)
{
    if ( (fd < 0) || (addr == NULL) )
        return -1;

    int status;
    status = lseek(fd, 0, SEEK_SET);
    checkery(status, "lseek", errno);
    status = read(fd, r, s);
    checkery(status, "read", errno);
    status = lseek(fd, 0, SEEK_SET);
    checkery(status, "lseek", errno);

    return 0;
}

/* Clear the shared memory region */
int AriaMap::clearfd(void)
{
    clear();
    writefd(mem, MSIZE);
    return 0;
}

/* *************************
 * ***** MAPPED MEMORY *****
 * *************************
 */

/* Setup shared memory mapped region */
int AriaMap::map(void)
{
    if ( fd < 0 )
        return -1;

    addr = (long *) mmap(NULL, MSIZE, MPROT, MFLAGS, fd, 0);
    if ( addr == MAP_FAILED ) {
        checkery(addr, "mmap", errno);
        return -1;
    }

    if ( is_first ) {
        clearfd();
        int status = mlock(addr, MSIZE);
        checkery(status, "mlock", errno);
    }

    return 0;
}

/* Unmap shared memory region */
int AriaMap::unmap(void)
{
    if ( fd < 0 )
        return -1;

    size_t len = length()-1;
    write(fd, mem, len*sizeof(long));

    int status = munmap(addr, MSIZE);
    checkery(status, "munmap", errno);
    close(fd);

    return 0;
}

/* *********************
 * ***** UTILITIES *****
 * *********************
 */

/* Print memory mapped data */ 
void AriaMap::print(void)
{
    size_t i;
    for ( i = 0; i < max_len; ++i )
        std::cout
            << "i: " << i
            << " | mem[i]: "
            << mem[i]
            << std::endl;
}

/* Find item in local mapped memory copy */
int AriaMap::find(long val)
{
    size_t i;
    for ( i = 0; i < max_len; ++i )
        if ( mem[i] == val )
            return i;
    return -1;
}

/* Size of the shared memory region */
int AriaMap::length(void)
{
    if ( (fd < 0) || (addr == NULL) )
        return -1;

    size_t i;
    for ( i = 0; i < max_len; ++i )
        if ( mem[i] == -1 )
            return i+1;
    return i;
}

/* Clean up used mapped memory */
int AriaMap::clean(void)
{
    std::cout << "Cleaning..." << std::endl;
    openfd();
    map();
    clear();
    readfd(mem, MSIZE);
    print();
    pid_t pid   = getpid();
    long  start = find(pid);
    clear(start, counter);

    size_t i;
    size_t j = 0;
    for ( i = start+counter; i < max_len; ++i ) {
        if ( mem[i] == -1 ) {
            std::cout << "break" << std::endl;
            std::cout << "start: " << start << std::endl;
            std::cout << "counter: " << counter << std::endl;
            break;
        }
        mem[start+j] = mem[i];
        mem[i]       = -1;
        ++j;
    }

    writefd(mem, MSIZE);
    print();
    unmap();
    start   = 0;
    counter = 0;

    return 0;
}

/* Clear local mapped memory copy */
int AriaMap::clear(void)
{
    if ( (fd < 0) || (addr == NULL) )
        return -1;

    size_t i;
    for ( i = 0; i < max_len; ++i )
        mem[i] = -1;

    return 0;
}

int AriaMap::clear(long start, long len)
{
    size_t size = start+len;
    size_t i;
    for ( i = start; (i < size) && (i < max_len); ++i )
        mem[i] = -1;
    return 0;
}
