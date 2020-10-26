
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Main file to run
 */

#include "libs.h"
#include "main.h"

int main(int argc, char* argv[])
{

	int loop = init_services();
	int choice = 0;

	if(loop != 0)
		return loop;

	loop = AUTOBOOT();
	return_to_navigator();

	// Main loop
	while(aptMainLoop())
	{

		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		// break in order to return to installer menu
		if(EXIT_KEYS)
			break;

		else if(loop != 0)
			break;

		else if(kDown & KEY_DOWN && choice < 1) {
			++choice;
			draw_arrow(choice, choice - 1);
		}

		else if(kDown & KEY_UP && choice > 0) {
			--choice;
			draw_arrow(choice, choice + 1);
		}

		else if(kDown & KEY_A) {
			// Return to installer menu
			switch(choice) {
				case 0:
					loop = installer_menu();
					return_to_navigator();
					break;
				case 1:
					break;
			}
		}
	}

	exit_services();
	return loop;

}

int init_services(void)
{

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	Result sharedRes;

	sharedRes = amInit();

	if(R_FAILED(sharedRes)) {
		// Bottom left
		print_error("Failed initializing AM module", sharedRes);
		pause_3ds();
		return -1;
	}

	sharedRes = fsInit();

	if(R_FAILED(sharedRes)) {
		print_error("Failed initializing FS module", sharedRes);
		pause_3ds();
		return -1;
	}

	return 0;

}

void exit_services(void)
{
	gfxExit();
	amExit();
}

void print_functions(void)
{
	formatted_print(" Cia installer", 0, 4);
	formatted_print(" Uninstaller", 0, 5);
}

void draw_arrow(int choice, int previous)
{
	formatted_print(" ", 0, previous + 4);
	formatted_print(">", 0, choice + 4);
}

void return_to_navigator(void)
{
	consoleClear();
	move_cursor(0, 0);

	formatted_print("Choose an action with [A]", 0, 2);
	print_functions();

	draw_arrow(0, 0);
}
