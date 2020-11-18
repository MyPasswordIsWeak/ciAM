
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

	if (total == -1)
		return -1;

	redraw_selected(line, selected);

	while (aptMainLoop())
	{

		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		// return -1 to indicate exit
		if (EXIT_KEYS)
			break;

		// Return to navigator
		else if (kDown & KEY_L)
			return 0;

		if (kDown & KEY_DOWN && selected < total)
		{
			++selected;
			redraw_selected(line, selected);
		}

		else if (kDown & KEY_UP && selected > 1)
		{
			--selected;
			redraw_selected(line, selected);
		}

		else if (kDown & KEY_A && selected != 0)
		{

			char filePath[(sizeof(char) * 128) + sizeof(char) * sizeof(CIA_DIR)] = CIA_DIR;

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
	u32 tidcount = 0;
	u64 *titleIds = malloc(1);
	AM_TitleEntry *entries = malloc(1);

	load_titles(MEDIATYPE_SD, &tidcount, titleIds, entries);

	u32 pageSize = (tidcount > 26) ? 26 : tidcount;
	u32 possiblePages = tidcount / pageSize;
	u32 vAlign = 0;
	u32 offset = 0;
	
	printf("\x1b[%lu;0H>\n", vAlign + 3);

	draw_page(offset, pageSize, tidcount, entries);	

	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (EXIT_KEYS)
			break;

		//													current page
		if (kDown & KEY_RIGHT && possiblePages != 0 && (offset / pageSize) < possiblePages)
		{
			offset += pageSize;
			draw_page(offset, pageSize, tidcount, entries);
			vAlign = 0;
			printf("\x1b[%lu;0H>\n", vAlign + 3);
		}

		//													current page
		else if (kDown & KEY_LEFT && possiblePages != 0 && (offset / pageSize) > 0)
		{
			offset -= pageSize;
			draw_page(offset, pageSize, tidcount, entries);
			vAlign = 0;
			printf("\x1b[%lu;0H>\n", vAlign + 3);
		}

		//											Max abs offset		Absolute offset
		else if (kDown & KEY_DOWN && (vAlign < 25 && (tidcount - 1) - (offset + vAlign) > 0))
		{
			++vAlign;
			printf("\x1b[%lu;0H \n", vAlign + 2);
			printf("\x1b[%lu;0H>\n", vAlign + 3);
		}

		else if (kDown & KEY_UP && vAlign > 0)
		{
			--vAlign;
			printf("\x1b[%lu;0H \n", vAlign + 4);
			printf("\x1b[%lu;0H>\n", vAlign + 3);
		}

		else if (kDown & KEY_A)
		{
			while(aptMainLoop())
			{
				char *productCode = calloc(16 + 1, sizeof(char));

				AM_GetTitleProductCode(MEDIATYPE_SD, titleIds[offset + vAlign], productCode);

				formatted_print("                          " , 24, 3);
				formatted_print(" ========================= " ,24, 4);
				formatted_print(" | Do you want to delete | ", 24, 5);
				printf("\x1b[6;24H | %016llX      | ", titleIds[offset + vAlign]);
				printf("\x1b[7;24H | (%s)?         | ", productCode);
				formatted_print(" | [A] Complete removal  | ", 24, 8);
				formatted_print(" | [X] Delete title      | ", 24, 9);
				formatted_print(" | [B] Return            | ", 24, 10);
				formatted_print(" ========================= ", 24, 11);
				formatted_print("                           " , 24, 12);

				while(aptMainLoop())
				{
					gspWaitForVBlank();
					gfxSwapBuffers();
					hidScanInput();

					u32 kDown = hidKeysDown();
					
					if (kDown & KEY_A)
					{
						delete_title(true, titleIds[offset + vAlign]);
						load_titles(MEDIATYPE_SD, &tidcount, titleIds, entries);
						draw_page(offset, pageSize, tidcount, entries);
						break;
					}
					else if (kDown & KEY_X)
					{
						delete_title(false, titleIds[offset + vAlign]);
						load_titles(MEDIATYPE_SD, &tidcount, titleIds, entries);
						draw_page(offset, pageSize, tidcount, entries);
						break;
					}
					else if (EXIT_KEYS)
					{
						draw_page(offset, pageSize, tidcount, entries);
						break;
					}
				}

				free(productCode);
				break;
			}
		}
	}

	free(entries);
	free(titleIds);
	return 0;
}

void draw_page(int offset, int pageSize, u32 tidcount, AM_TitleEntry *entries)
{
	consoleClear();
	char *productCode = calloc(16 + 1, sizeof(char));

	for (int i = offset; i < pageSize + offset; ++i)
	{

		Result res = AM_GetTitleProductCode(MEDIATYPE_SD, entries[i].titleID, productCode);

		if (R_FAILED(res))
		{
			print_error("Failed to get product code for title - ", res);
		}

		if (i < tidcount)
			printf("\x1b[%i;0H  %016llx|%6.1fM|%s\n", i - offset + 3, entries[i].titleID, (float)entries[i].size / 1024 / 1024, productCode);
		else
			break;
	}

	free(productCode);
}

int delete_title (bool deleteticket, u64 titleid) 
{
	formatted_print("Deleting ...", 25, 12);
	Result res;
	 
	res = AM_DeleteTitle(get_title_location(titleid), titleid);
	
	if (R_FAILED(res))
	{
		print_error("Failed to delete title", res);
		pause_3ds();
		return -1;
	}

	if (deleteticket)
	{
		res = AM_DeleteTicket(titleid);

		if (R_FAILED(res))
		{
			print_error("Failed to delete ticket", res);
			pause_3ds();
			return -1;
		}
	}

	return 0;
}

int load_titles(FS_MediaType mediaType, u32 *titleCount, u64 *titleIds, AM_TitleEntry *titleEntries)
{
	Result res;
	u64 *tids;
	AM_TitleEntry *entries;

	//Get Title Count
	res = AM_GetTitleCount(mediaType, titleCount);

	if (R_FAILED(res))
	{
		print_error("Failed to get title count.", res);
		pause_3ds();
		return -1;
	}

	// Imagine trying on Citra LMZO
	if (*titleCount == 0)
	{
		printf("No titles found.");
		return -1;
	}

	tids = calloc(*titleCount, sizeof(u64));

	if (tids == NULL)
	{
		printf("Out of memory. Tried to allocate memory for title ids.");
		return -1;
	}

	entries = calloc(*titleCount, sizeof(AM_TitleEntry));

	if (tids == NULL)
	{
		printf("Out of memory. Tried to allocate memory for title ids.");
		return -1;
	}

	//Get Title IDs
	res = AM_GetTitleList(titleCount, mediaType, *titleCount, tids);

	if (R_FAILED(res))
	{
		print_error("Failed to get Title IDs.", res);
		pause_3ds();
		return -1;
	}

	res = AM_GetTitleInfo(mediaType, *titleCount, tids, entries);

	if (R_FAILED(res))
	{
		print_error("Failed to get Title Entries.", res);
		pause_3ds();
		return -1;
	}

	titleIds = tids;
	titleEntries = entries;

	return 0;
}