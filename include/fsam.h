
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Header for fsam.c
 */

#ifndef __FS_H__
#define __FS_H__

#include <dirent.h>
#include "libs.h"

char *get_item_in_dir(char *directory, int number);
FS_MediaType getTitleDestination(u64 titleId);
int install_cia(char *path, int line, u8 ask);
int list_diritems(char *directory);

#endif
