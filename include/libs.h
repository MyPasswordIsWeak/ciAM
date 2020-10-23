
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: To load common libs and definitions
 */

#ifndef __LIBS_H__
#define __LIBS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "util.h"
#include "fs.h"

// If you want you can change this
#define EXIT_KEYS (kDown & KEY_START || kDown & KEY_B)
#define INSTALL_BUFFER_SIZE 0x200000
#define DEBUG_FILE "/ciAM_debug.log"
#define VERSION "v1.0.0"
#define CIA_DIR "/cias/"
#define WANTS_DEBUG true

#endif
