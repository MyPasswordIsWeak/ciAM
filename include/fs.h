
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Header for fs.c
 */

#ifndef __FS_H__
#define __FS_H__

#include <dirent.h>

char *get_item_in_dir(DIR *directory, int number);
FS_MediaType getTitleDestination(u64 titleId);
int install_cia(char *path, int line, u8 ask);
int list_diritems(DIR *directory);

#endif
