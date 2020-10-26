
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: To load common libs and definitions
 */

#ifndef __LIBS_H__
#define __LIBS_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <3ds.h>

#include "actions.h"
#include "util.h"
#include "fsam.h"

// If you want you can change this
#define EXIT_KEYS (kDown & KEY_START || kDown & KEY_B)
#define INSTALL_BUFFER_SIZE 0x200000
#define DEBUG_FILE "/CiAM_debug.log"
#define AUTOBOOT uninstaller_menu
#define VERSION "v1.0.0"
#define CIA_DIR "/cias/"
#define WANTS_DEBUG

#endif
