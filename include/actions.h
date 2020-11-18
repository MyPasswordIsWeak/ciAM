
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: header for actions.c
 */

#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "libs.h"


int load_titles(FS_MediaType mediaType, u32 *tidsc, u64 *titleIds, AM_TitleEntry *titleEntries);
void draw_page(int offset, int pageSize, u32 tidcount, AM_TitleEntry *entries);
int delete_title(bool deleteticket, u64 titleid);
int uninstaller_menu(void);
int installer_menu(void);

#endif
