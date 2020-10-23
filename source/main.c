
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Main file to run
 */

#include "main.h"

int main(int argc, char* argv[])
{

	int loopControl = init_services();

	if(loopControl == -1)
		return -1;

	print_usage();

	int total = list_diritems(CIA_DIR);
	// = total + (Usage + offset)
	int line = total + 10;
	int selected = 0;

	if(line == -1)
		return -1;

	redraw_selected(line, selected);

	// Main loop
	while(aptMainLoop())
	{

		if(loopControl == -1)
			break;

		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		// break in order to return to hbmenu
		if(EXIT_KEYS)
			break;

		if(kDown & KEY_DOWN && selected < total) {
			++selected;
			redraw_selected(line, selected);
		}

		else if(kDown & KEY_UP && selected > 1) {
			--selected;
			redraw_selected(line, selected);
		}

		else if((kDown & KEY_R && kDown & KEY_A)) {

			DIR *items;
			struct dirent *item;
			int res = read_directory(CIA_DIR, &items);

			if(res != -1) {
				while((item = readdir(items))) {

					char current[128 + sizeof(CIA_DIR)] = CIA_DIR;
					strcat(current, item->d_name);

					debug(format("Installing file: %s", current));

					install_cia(current, line, 0);
					clean_screen();
					redraw_selected(line, selected);

				}
				free(items);
			} else {
				formatted_print(format("Couldn't open directory (%s)", CIA_DIR), 0, 29);
			}
		}

		else if(kDown & KEY_A && selected != 0) {

			char selectedFileRel[128] = "";
			char selectedFileAbs[128 + sizeof(CIA_DIR)] = CIA_DIR;

			strcpy(selectedFileRel, get_item_in_dir(CIA_DIR, selected - 1));
			strcat(selectedFileAbs, selectedFileRel);

			debug(format("Installing file: %s", selectedFileAbs));

			install_cia(selectedFileAbs, line, 1);
			clean_screen();
			redraw_selected(line, selected);

		}

	}

	exit_services();
	return loopControl;

}

void redraw_selected(int line, int selected)
{
	printf("\x1b[%i;0HSelected file: %i\n", line, selected);
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
