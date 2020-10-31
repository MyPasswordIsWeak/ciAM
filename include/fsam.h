
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
int install_cia(char *path, int line, bool ask);
FS_MediaType get_title_location(u64 titleId);
int list_diritems(char *directory);

#endif
