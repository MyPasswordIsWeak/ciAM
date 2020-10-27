
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

			install_cia(filePath, line, true);
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

	// Imagine trying on Citra LMZO
	if(tidcount == 0)
		return 0;

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
	int vAlign = 0;
	int offset = 0;

	draw_page(offset, pageSize, tidcount, tids);
	printf("\x1b[%i;0H>\n", vAlign + 3);

	while(aptMainLoop()) {

		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if(EXIT_KEYS)
			break;

		//													current page
		if(kDown & KEY_RIGHT && possiblePages != 0 && (offset / pageSize) < possiblePages) {
			offset += pageSize;
			draw_page(offset, pageSize, tidcount, tids);
			vAlign = 0;
			printf("\x1b[%i;0H>\n", vAlign + 3);
		}

		//													current page
		else if(kDown & KEY_LEFT && possiblePages != 0  && (offset / pageSize) > 0) {
			offset -= pageSize;
			draw_page(offset, pageSize, tidcount, tids);
			vAlign = 0;
			printf("\x1b[%i;0H>\n", vAlign + 3);
		}

		//										current page
		else if(kDown & KEY_DOWN && vAlign < ((offset / pageSize) == possiblePages ? (tidcount % 25) - 2 : 25)) {
			++vAlign;
			printf("\x1b[%i;0H \n", vAlign + 2);
			printf("\x1b[%i;0H>\n", vAlign + 3);
		}

		else if(kDown & KEY_UP && vAlign > 0) {
			--vAlign;
			printf("\x1b[%i;0H \n", vAlign + 4);
			printf("\x1b[%i;0H>\n", vAlign + 3);
		}

		else if(kDown & KEY_A) {
			bool deltik = true;
			formatted_print("Are you sure you", 25, 5);
			formatted_print("Want to delete", 25, 6);
			printf("\x1b[7;25H%016llx?", tids[offset + vAlign]);
			formatted_print("[A] Complete removal", 25, 8);
			formatted_print("[X] Delete title", 25, 9);
			formatted_print("[B] Return", 25, 10);
			while(aptMainLoop()) {
				gspWaitForVBlank();
				gfxSwapBuffers();
				hidScanInput();
				u32 kDown = hidKeysDown();
				if(kDown & KEY_A)
					break;
				else if(kDown & KEY_X) {
					deltik = false;
					break;
				}
				if(EXIT_KEYS)
					return 0;
			}

			formatted_print("Deleting ...", 25, 12);
			res = AM_DeleteTitle(get_title_location(tids[offset + vAlign]), tids[offset + vAlign]);
			if(R_FAILED(res)) {
				print_error("Failed to delete title", res);
				pause_3ds();
				return 0;
			}

			if(deltik) {
				res = AM_DeleteTicket(tids[offset + vAlign]);
				if(R_FAILED(res)) {
					print_error("Failed to delete title", res);
					pause_3ds();
					return 0;
				}
			}

			break;
		}

	}

	free(tids);
	return 0;
}

void draw_page(int offset, int pageSize, u32 tidcount, u64 *tids)
{
	consoleClear();
	for(int i = offset; i < pageSize + offset; ++i) {
		if(i < tidcount)
			printf("\x1b[%i;0H  %016llx\n", i - offset + 3, tids[i]);
		else
			break;
	}
}
