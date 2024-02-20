/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-05 11:21:11
 * @ Modified time: 2024-02-20 11:39:04
 * @ Description:
 *    Typedefs some custom types.
 *    We will be using lowercase letters and underscores to denote that something is a data type.
 * 
 *    Note that some of these data types take inspiration from the Windows data types 
 *        (bad idea tho, Microsoft creates its own names for everything).
 * 
 *    They are reimplemented here for the sake of convenience.
 *    Refer to the following documentation for more info: 
 *        https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
 */

#ifndef UTILS_TYPES
#define UTILS_TYPES

// You are in Windows
#ifdef _WIN32
#include "./win/utils.types.win.h"

// Not in Windows
#else
#include "./unix/utils.types.unix.h"
#endif

// Some primitives
// Gives us fixed-width data types
//    (1) uint8_t
//    (2) uint16_t
//    (3) uint32_t
#include <stdint.h>

#endif