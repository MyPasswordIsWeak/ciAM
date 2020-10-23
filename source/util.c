
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Some utilities to make my life easier
 */

#include "libs.h"
#include "util.h"

void formatted_print(char *text, int x, int y)
{
	printf("\x1b[%i;%iH%s\n", y, x, text);
}

void pause_3ds(void)
{

	// Middle-ish
	formatted_print("Press [START] or [B] to continue ...", 7, 26);
	while(aptMainLoop()) {

		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if(EXIT_KEYS)
			break;

	}

}

void debug(char *message)
{
	
	FILE *fd = fopen(DEBUG_FILE, "a");

	fprintf(fd, "debug: %s\n", message);
	fclose(fd);

}

void move_cursor(int x, int y)
{
	printf("\x1b[%i;%iH\n", y, x);
}

char *format(char *base, const void *appendum) {
	char *ret = malloc(128);
	sprintf(ret, base, appendum);
	return ret;
}

void print_error(char *message, long unsigned int error)
{
	printf("\x1b[28;0H%s", message);
	printf("\x1b[29;0H%08lX\n", error);
}

void clean_screen(void)
{
	consoleClear();
	move_cursor(0, 0);
	print_usage();
	list_diritems(CIA_DIR);
	debug("Cleaned screen");
}

void print_usage(void)
{

	formatted_print(format("Usage manual for ciAM version %s", VERSION), 0, 1);
	formatted_print("Key mappings are in [KEY] format", 0, 2);

	formatted_print("[A] Install selected cia", 0, 4);
	formatted_print("[B] Exit program", 0, 5);
	formatted_print("[D] Select cia", 0, 6);
	formatted_print("[U] Select cia", 0, 7);

}

// This can probably be done better, idc
void remove_pause(void)
{
	formatted_print("                                    ", 7, 26);
}
