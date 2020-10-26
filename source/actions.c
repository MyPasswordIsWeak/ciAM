
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: File which contains controllers for actions
 */

#include "actions.h"
#include "libs.h"

int installer_menu(void)
{

	print_usage();

	int total = list_diritems(CIA_DIR);
	// = total + (Usage + offset)
	int line = total + 10;
	int selected = 0;

	if(total == -1)
		return -1;

	redraw_selected(line, selected);

	while(aptMainLoop()) {

		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		// return -1 to indicate exit
		if(EXIT_KEYS)
			return -1;

		// Return to navigator
		else if(kDown & KEY_L)
			return 0;

		if(kDown & KEY_DOWN && selected < total) {
			++selected;
			redraw_selected(line, selected);
		}

		else if(kDown & KEY_UP && selected > 1) {
			--selected;
			redraw_selected(line, selected);
		
		}

		else if(kDown & KEY_A && selected != 0) {

			char filePath[(sizeof(char) * 128)  + sizeof(char) * sizeof(CIA_DIR)] = CIA_DIR;

			strcat(filePath, get_item_in_dir(CIA_DIR, selected - 1));
			debug(format("Installing file: %s", filePath));

			install_cia(filePath, line, 1);
			clean_screen();
			redraw_selected(line, selected);

		}
	}
	return 0;
}

int batch_installer_menu(void)
{

	// formatted_print(format("Installing all files from %s", CIA_DIR), 0, 29);
	// DIR *cias = opendir(CIA_DIR);
	// struct dirent *item;

	// while((item = readdir(cias))) {

	// 	char filePath[(sizeof(char) * 128)  + (sizeof(char) * sizeof(CIA_DIR))] = CIA_DIR;

	// 	strcat(filePath, item->d_name);
	// 	debug(format("Installing file: %s", filePath));

	// 	install_cia(filePath, line, 0);
	// 	clean_screen();
	// 	redraw_selected(line, selected);

	// }
	// closedir(cias);
	return 0;
}

int uninstaller_menu(void)
{
	u32 tidcount;
	Result res;

	res = AM_GetTitleCount(MEDIATYPE_SD, &tidcount);

	if(R_FAILED(res)) {
		print_error("Failed getting total titles count", res);
		pause_3ds();
		return 0;
	}

	u64 *tids = calloc(tidcount, sizeof(u64));
	res = AM_GetTitleList(&tidcount, MEDIATYPE_SD, tidcount, tids);

	if(R_FAILED(res)) {
		print_error("Failed to get titles", res);
		pause_3ds();
		return 0;
	}

	int pageSize = 26 < tidcount
		? 26
		: tidcount;
	int possiblePages = tidcount / pageSize;
	int currentOffset = 0;

	printf("\x1b[0;0HPossible pages: %i\n", possiblePages);
	for(int i = currentOffset; i < pageSize; ++i)
		printf("\x1b[%i;0H%llx\n", i + 3, tids[i]);

	while(aptMainLoop()) {

		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if(EXIT_KEYS)
			break;

		//								current page		total possible
		if(kDown & KEY_RIGHT && (currentOffset / pageSize) < possiblePages ) {
			consoleClear();
			currentOffset += pageSize;
			printf("\x1b[0;0HPossible pages: %i", possiblePages);
			for(int i = currentOffset; i < pageSize + currentOffset; ++i)
				printf("\x1b[%i;0H%llx\n", i + 3, tids[i]);
		}

		//								current page
		if(kDown & KEY_LEFT && (currentOffset / pageSize) > 0) {
			consoleClear();
			currentOffset -= pageSize;
			printf("\x1b[0;0HPossible pages: %i", possiblePages);
			for(int i = currentOffset; i < pageSize + currentOffset; ++i)
				printf("\x1b[%i;0H%llx\n", i + 3, tids[i]);
		}

	}

	free(tids);
	return 0;
}
