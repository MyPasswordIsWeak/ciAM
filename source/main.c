
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: Main file to run
 */

#include "main.h"

int main(int argc, char* argv[])
{

	int loopControl = init_services();
	DIR *cias = opendir(CIA_DIR);

	if(loopControl == -1)
		return -1;

	print_usage();

	int total = list_diritems(cias);
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

		else if((kDown & KEY_R) && (kDown & KEY_A)) {

			formatted_print(format("Installing all files from %s", CIA_DIR), 0, 29);
			struct dirent *item;

			while((item = readdir(cias))) {

				char *filePath = malloc((sizeof(char) * 128)  + sizeof(char) * sizeof(CIA_DIR));

				strcat(filePath, CIA_DIR);
				strcat(filePath, item->d_name);
				debug(format("Installing file: %s", filePath));

				install_cia(filePath, line, 0);
				clean_screen(cias);
				redraw_selected(line, selected);

			}

			rewinddir(cias);
		}

		else if(kDown & KEY_A && selected != 0) {

			char *filePath = malloc((sizeof(char) * 128)  + sizeof(char) * sizeof(CIA_DIR));

			strcat(filePath, CIA_DIR);
			strcat(filePath, get_item_in_dir(cias, selected - 1));
			debug(format("Installing file: %s", filePath));

			install_cia(filePath, line, 1);
			free(filePath);
			clean_screen(cias);
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
