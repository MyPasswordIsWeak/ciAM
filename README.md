
# CiAM

Cia Application Manager (CiAM) - A minimalist cia installer for the 3ds  
a combination of the AM (application module) module and cia (ctr importable archive)  

# Controls

|Button|Function|
|-----|-----|
|A|Select/continue|
|B|Exit/Cancel|
|UP|Select up|
|DOWN|Select down|
|START|Exit/Cancel|

# Building

1) Install devkitARM
2) Run `make -j4` in project dir
3) You will see a compiled 3dsx appear

You might want to change a few values in include/libs.h:
|Name|What does it do|
|-----|-----|
|EXIT_KEYS|The keys used to exit, wont change the text value, for keys look [here](https://libctru.devkitpro.org/hid_8h.html#a2f80701c36e79c0640d91c788feee0b3aa7b3a3d0503a46bec51c6e3da95747cc)|
|INSTALL_BUFFER_SIZE|The buffer sized to installing, you probably don't want to change this|
|DEBUG_FILE|The file to debug to, if you turned off WANTS_DEBUG, this does nothing, else its the log file location, / is the root of the sd|
|VERSION|The version, this will change the number shown in the start menu|
|CIA_DIR|The folder to look for cias in, / is the root just like DEBUG_FILE|
|WANTS_DEBUG|Comment this out if you don't want to create log files (// to comment)|
|AUTOBOOT|The function to autoboot, all menu functions are easy to find in actions.h|
