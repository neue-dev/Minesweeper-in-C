/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-05 11:18:06
 * @ Modified time: 2024-02-29 21:41:41
 * @ Description:
 * 
 * A utility library for implementing some string related functionality
 *    on Windows.
 * 
 * The funny thing is I kinda only need this for printing strings, because
 *    for some damn reason puts() is more reliable on Windows and printf() is
 *    more reliable on Linux (at least based from my tests).
 */

#ifndef UTILS_STRING_WIN_
#define UTILS_STRING_WIN_

#include <stdio.h>

void String_print(char *string) {
  puts(string);
}

#endif