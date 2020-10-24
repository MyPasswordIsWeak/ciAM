
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Header for util.c
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <dirent.h>

char *format(char *base, const void *appendum);
void formatted_print(char *text, int x, int y);
void print_error(char *message, Result error);
void move_cursor(int x, int y);
void debug(char *message);
void clean_screen(void);
void remove_pause(void);
void print_usage(void);
void pause_3ds(void);

#endif
