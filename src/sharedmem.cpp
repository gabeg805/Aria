/* *****************************************************************************
 * 
 * Name:    AriaSharedMem.cc
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

/* Includes */
#include "sharedmem.hpp"
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
static const  size_t  MSIZE   = MLEN * sizeof(struct SharedMemType);
static        long   *MADDR   = NULL;
static const  mode_t  FMODE   = 0777;
static        int     FFLAGS  = O_RDWR;
static        int     FD      = -1;
static struct SharedMemType MEM[MLEN];

/* ************************************************************************** */
/**
 * @brief Interface to save information in shared memory.
 * 
 * @details Executes the whole process of mapping the shared memory region,
 *          storing the data, unmapping, and cleanup.
 * 
 * @param data information to save in shared memory.
 * 
 * @param shift number of pixels to separate elements that overlap.
 */
int AriaSharedMem::add(struct SharedMemType *data, long shift)
{
    int status;
    if ( (status=AriaSharedMem::memopen()) < 0 )
        return -1;
    if ( (status=AriaSharedMem::update()) < 0 )
        return status;
    if ( (status=AriaSharedMem::displace(data, shift)) < 0 )
        return status;
    if ( (status=AriaSharedMem::append(data)) < 0 )
        return status;
    if ( (status=AriaSharedMem::memclose()) < 0 )
        return status;

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Cleanup shared memory that is no longer being used.
 * 
 * @details Cleanup shared memory that is no longer being used and reorder the
 *          shared memory region so that used memory is at the start.
 */
int AriaSharedMem::remove(void)
{
    AriaSharedMem::memopen();
    AriaSharedMem::update();
    AriaSharedMem::clearcurrent();
    AriaSharedMem::reorder();
    AriaSharedMem::memclose();

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Return the first element in shared memory.
 */
struct SharedMemType * AriaSharedMem::getfirst(void)
{
    int len = AriaSharedMem::length();
    return ( len < 0 ) ? NULL : &MEM[0];
}

/* ************************************************************************** */
/**
 * @brief Return the last element in shared memory.
 */
struct SharedMemType * AriaSharedMem::getlast(void)
{
    int len = AriaSharedMem::length();
    return ( len < 0 ) ? NULL : &MEM[len-1];
}

/* ************************************************************************** */
/**
 * @brief Open a file descriptor to the shared memory region.
 * 
 * @details Open a file descriptor to the shared memory region. When the file
 *          descriptor is determined, set a mutex on the file descriptor so that
 *          there are no collisions between other instances of Aria.
 */
int AriaSharedMem::memopen(void)
{
    if ( FD >= 0 )
        return 0;

    bool created = false;
    if ( access(MFILE, F_OK) < 0 ) {
        FFLAGS  |= O_CREAT;
        created  = true;
    }

    if ( (FD=open(MFILE, FFLAGS, FMODE)) < 0 ) {
        // AriaUtility::errprint("open", errno);
        return -1;
    }
    if ( flock(FD, LOCK_EX) < 0 ) {
        // AriaUtility::errprint("flock", errno);
        return -1;
    }
    if ( AriaSharedMem::memmap() < 0 ) {
        // AriaUtility::errprint("memmap: error.");
        return -1;
    }
    if ( created )
        memclear();

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Read from the shared memory region.
 * 
 * @details Read the number of bytes, beginning from the start of the shared
 *          memory region.
 */
int AriaSharedMem::memread(struct SharedMemType *r, size_t s)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return -1;

    if ( lseek(FD, 0, SEEK_SET) < 0 ) {
        // AriaUtility::errprint("lseek", errno);
        return -1;
    }
    if ( read(FD, r, s) < 0 ) {
        // AriaUtility::errprint("read", errno);
        return -1;
    }
    if ( lseek(FD, 0, SEEK_SET) < 0 ) {
        // AriaUtility::errprint("lseek", errno);
        return -1;
    }
    return 0;
}

/* ************************************************************************** */
/**
 * @brief Write data to the shared memory region.
 * 
 * @details Write data to the shared memory region, specifying how much data to
 *          write.  This write will begin at the start of the shared memory
 *          region.
 */
int AriaSharedMem::memwrite(struct SharedMemType *w, size_t s)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return -1;

    if ( lseek(FD, 0, SEEK_SET) < 0 ) {
        // AriaUtility::errprint("lseek", errno);
        return -1;
    }
    if ( write(FD, w, s) < 0 ) {
        // AriaUtility::errprint("write", errno);
        return -1;
    }
    if ( lseek(FD, 0, SEEK_SET) < 0 ) {
        // AriaUtility::errprint("lseek", errno);
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Close the shared memory region.
 * 
 * @details Upon closing the shared memory region, write the local copy data to
 *          shared memory, unmap the memory region, and close the file
 *          descriptor.
 */
int AriaSharedMem::memclose(void)
{
    if ( AriaSharedMem::memwrite(MEM, MSIZE) < 0 ) {
        // AriaUtility::errprint("memwrite: error.");
    }
    if ( AriaSharedMem::memunmap() < 0 ) {
        // AriaUtility::errprint("memunmap: error.");
    }
    int status = close(FD);
    FD         = -1;
    return status;
}

/* ************************************************************************** */
/**
 * @brief Clear the shared memory region.
 * 
 * @details Clear the shared memory region by writing 0 to the local copy and
 *          the actual shared memory region.
 */
int AriaSharedMem::memclear(void)
{
    AriaSharedMem::clear();
    return AriaSharedMem::memwrite(MEM, MSIZE);
}

/* ************************************************************************** */
/**
 * @brief Map the shared memory region.
 */
int AriaSharedMem::memmap(void)
{
    if ( (FD < 0) )
        return -1;

    MADDR = (long *) mmap(NULL, MSIZE, MPROT, MFLAGS, FD, 0);
    if ( MADDR == MAP_FAILED ) {
        // AriaUtility::errprint("mmap", errno);
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Unmap the shared memory region.
 */
int AriaSharedMem::memunmap(void)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return -1;

    if ( munmap(MADDR, MSIZE) < 0 )
        // AriaUtility::errprint("munmap", errno);

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Update the local copy by writing the data currently in shared memory
 *        to the local copy.
 */
int AriaSharedMem::update(void)
{
    int status = memread(MEM, MSIZE);
    return status;
}

/* ************************************************************************** */
/**
 * @brief Append data to the local copy of the shared memory region.
 * 
 * @param data information to store in shared memory.
 */
int AriaSharedMem::append(struct SharedMemType *data)
{
    int len = AriaSharedMem::length();
    if ( (FD < 0) || (MADDR == NULL) || (len == MLEN) )
        return -1;

    size_t index = (len == 0) ? 0 : len;
    MEM[index]   = *data;

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Insert data to the local copy of the shared memory region.
 * 
 * @param data information to store in shared memory.
 * 
 * @param index location in which to store memory in the local copy.
 */
int AriaSharedMem::insert(struct SharedMemType *data, size_t index)
{
    if ( (FD < 0) || (MADDR == NULL) || (index >= MLEN) )
        return -1;

    MEM[index]   = *data;

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
 * @param data information to save in shared memory.
 * 
 * @param shift number of pixels to separate elements that overlap.
 */
int AriaSharedMem::displace(struct SharedMemType *data, long shift)
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
 * @brief Find an element in shared memory.
 * 
 * @details Searches for a match between the local copy data element's PID and
 *          the given value.
 * 
 * @param id the PID to match with the local copy data.
 */
int AriaSharedMem::find(long id)
{
    size_t i;
    for ( i = 0; i < MLEN; ++i )
        if ( MEM[i].id == id )
            return i;
    return -1;
}

/* ************************************************************************** */
/**
 * @brief Find the shared memory used in the current session.
 */
int AriaSharedMem::findcurrent(void)
{
    pid_t pid = getpid();
    size_t i;
    for ( i = 0; i < MLEN; ++i )
        if ( MEM[i].id == pid )
            return i;
    return -1;
}

/* ************************************************************************** */
/**
 * @brief Reorder the local copy of the shared memory region to ensure there are
 *        no holes alongside contiguous blocks of memory.
 */
int AriaSharedMem::reorder(void)
{
    size_t i;
    int    j;
    for ( i = 0, j = -1; i < MLEN; ++i ) {
        if ( MEM[i].id == 0 ) {
            if ( j == -1 )
                j = i;
            continue;
        }
        else {
            if ( j == -1 )
                continue;
            MEM[j] = MEM[i];
            AriaSharedMem::clear(i);
            i = j;
            j = -1;
        }
    }

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Clear the local copy of the shared memory region.
 */
int AriaSharedMem::clear(void)
{
    memset(MEM, 0, MSIZE);
    return 0;
}

/* ************************************************************************** */
/**
 * @brief Clear data at the given index of the local copy of the shared memory
 *        region.
 * 
 * @param index the index in the local copy corresponding to the data to clear.
 */
int AriaSharedMem::clear(size_t index)
{
    if ( index >= MLEN )
        return -1;

    memset(&MEM[index], 0, sizeof(*MEM));

    return 0;
}

/* ************************************************************************** */
/**
 * @brief Clear the data that was added in the current session from the local
 *        copy of the shared memory region.
 */
int AriaSharedMem::clearcurrent(void)
{
    int index = AriaSharedMem::findcurrent();
    if ( index < 0 )
        return -1;
    AriaSharedMem::clear(index);

    return 0;
}

/* ************************************************************************** */
/**
 * @brief The size of the shared memory region.
 */
size_t AriaSharedMem::size(void)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return 0;

    int len = AriaSharedMem::length();
    return (len == 0) ? 0 : (len-1)*sizeof(*MEM);
}

/* ************************************************************************** */
/**
 * @brief The length of the shared memory region.
 * 
 * @details The number of elements (data chunks) in the shared memory region.
 */
size_t AriaSharedMem::length(void)
{
    if ( (FD < 0) || (MADDR == NULL) )
        return 0;

    size_t i;
    for ( i = 0; i < MLEN; ++i )
        if ( MEM[i].id == 0 )
            break;
    return i;
}

/* ************************************************************************** */
/**
 * @brief Return whether the local copy of the shared memory region is empty or
 *        not.
 * 
 * @details The number of elements (data chunks) in the shared memory region.
 */
bool AriaSharedMem::isempty(void)
{
    if ( AriaSharedMem::memopen() < 0 )
        return true;
    if ( AriaSharedMem::update() < 0 )
        return true;

    return (AriaSharedMem::length() == 0) ? true : false;
}

/* ************************************************************************** */
/**
 * @brief Print the contents of the shared memory region.
 */
void AriaSharedMem::print(void)
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


/* ************************************************************************** */
/**
 * @brief Print the contents of the shared memory region at a specific index.
 * 
 * @param index the index in the local copy to print out.
 */
void AriaSharedMem::print(size_t index)
{
    std::cout
        << "index: " << index
        << " | MEM[index]: "
        << MEM[index].id
        << " "
        << MEM[index].x
        << " "
        << MEM[index].y
        << " "
        << MEM[index].w
        << " "
        << MEM[index].h
        << std::endl;
}

/* ************************************************************************** */
/**
 * @brief Print the contents of the shared memory data chunk.
 * 
 * @param data the shared memory data chunk to print out.
 */
void AriaSharedMem::print(struct SharedMemType *data)
{
    std::cout
        << "DATA: "
        << data->id
        << " "
        << data->x
        << " "
        << data->y
        << " "
        << data->w
        << " "
        << data->h
        << std::endl;
}
