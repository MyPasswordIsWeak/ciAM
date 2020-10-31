
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: header for actions.c
 */

#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "libs.h"

void draw_page(int offset, int pageSize, u32 tidcount, u64 *tids);
int uninstaller_menu(void);
int installer_menu(void);

#endif
