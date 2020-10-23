
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Header for util.c
 */

#ifndef __UTIL_H__
#define __UTIL_H__

void print_error(char *message, long unsigned int error);
char *format(char *base, const void *appendum);
void formatted_print(char *text, int x, int y);
void move_cursor(int x, int y);
void debug(char *message);
void remove_pause(void);
void clean_screen(void);
void print_usage(void);
void pause_3ds(void);

#endif
