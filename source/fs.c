
/*
 * Author: MyPasswordIsWeak
 * License: MIT
 * Purpose: All FS related shenanigance, now including cia installation, so also AM
 */

#include "libs.h"
#include "fs.h"

/*
 * Params:
 *	directory	- The directory to read
 *  *&targetDir	- An array to read into
 * Returns:
 *  1 - success
 * -1 - failed
*/
int read_directory(char *directory, DIR **targetDir)
{

	// Open directory
	*targetDir = opendir(directory);

	if(targetDir == NULL)
		return -1;

	return 0;

}

/*
 * Example res:
 *  1) SomeFile.cia
 *	2) SomeSecondFile.cia
 * Params:
 *	*directory	- The directory to read
 * Returns:
 * -1 - failed
 *  * - Amount of files in directory
 */
int list_diritems(char *directory)
{

	DIR *items;
	int res = read_directory(directory, &items);

	if(res == -1) {
		formatted_print(format("Couldn't open directory (%s)", directory), 0, 29);
		return -1;
	}

    struct dirent *item;
    int count = 1;

	while((item = readdir(items))) {

		/* COUNT) FILENAME */
		printf("\x1b[%i;0H%i) %s", count + 8, count, item->d_name);
		++count;

	}

	free(items);
	printf("\n");
	return count - 1;

}

char *get_item_in_dir(char *directory, int number)
{

	DIR *items;
	int res = read_directory(directory, &items);

	if(res == -1) {
		formatted_print(format("Couldn't open directory (%s)", directory), 0, 29);
		return "";
	}

	char *ret = malloc(sizeof(char) * 128);
    struct dirent *item;
    int count = 0;

	while((item = readdir(items))) {
		if(count == number)
			strcpy(ret, item->d_name);
		++count;
	}

	free(items);
	return ret;

}

/*
 * path: the path to open a handle to
 * handle: A ref to a handle to write to
 */
int create_file_handle_read(char *path, Handle *handle)
{

	FS_ArchiveID id = ARCHIVE_SDMC;
	u32 flags = FS_OPEN_READ;
	u32 attribs = 0;

	Result res;

	FS_Path pathToArchive = fsMakePath(PATH_EMPTY, "");
	FS_Path pathToFile = fsMakePath(PATH_ASCII, path);

	res = FSUSER_OpenFileDirectly(handle, id, pathToArchive, pathToFile, flags, attribs);
	if(R_FAILED(res)) {
		print_error("Failed opening a handle for the file", res);
		pause_3ds();
		return -1;
	}

	return 0;

}

// https://github.com/Universal-Team/Universal-Updater/blob/master/source/utils/cia.cpp#L67
// Thanks for showing how2install cias
int install_cia(char *path, int line, u8 ask)
{

	line = line + 2;

	FS_MediaType media = MEDIATYPE_SD;
	AM_TitleEntry title;

	Handle file;
	Result res;

	int fileHandleRes = create_file_handle_read(path, &file);
	if(fileHandleRes == -1)
		return -1;

	res = AM_GetCiaFileInfo(media, &title, file);

	if(R_FAILED(res)) {
		print_error("Failed reading cia metadata", res);
		pause_3ds();
		return -1;
	}

	// lu = u32, 016llx = 16 length hex (thanks Timm)
	printf("\x1b[%i;0HTitle ID: %016llx", line, title.titleID);
	printf("\x1b[%i;0HVersion: %i", line + 1, title.version);
	printf("\x1b[%i;0HSize: %lluMiB", line + 2, title.size / 1024 / 1024);

	if(ask == 1) {
		formatted_print("Press [B] to cancel or [A] to continue ...", 4, 26);
		while(aptMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown();
			if(kDown & KEY_B)
				return 0;
			else if(kDown & KEY_A)
				break;
		}
		formatted_print("                                           ", 4, 26);
	}

	// Real stuff starts here
	media = getTitleDestination(title.titleID);

	// For you Timm, since you hate commas
	u32 written;
	u32 read;

	u32 offset = 0;
	u64 size = 0;
	Handle cia;

	res = AM_StartCiaInstall(media, &cia);

	if (R_FAILED(res)) {
		print_error("Failed to get filesize", res);
		pause_3ds();
		return -1;
	}

	res = FSFILE_GetSize(file, &size);
	if (R_FAILED(res)) {
		printf("Error in:\nFSFILE_GetSize\n");
		return -1;
	}

	// INSTALL_BUFFER_SIZE --> libs.h
	u8 *buffer = malloc(INSTALL_BUFFER_SIZE);
	// i dont think this should happen
	if(buffer == NULL) {
		// = 000-1011 (Out of memory)
		print_error("Failed allocating memory", 0xD8C3FBF3);
		pause_3ds();
		return -1;
	}

	debug("Starting cia install with total install");
	printf("\x1b[29;0HPercent: 0%% (0MiB/unk)");

	do {

		// So basically read file and then write to cia handle
		FSFILE_Read(file, &read, offset, buffer, INSTALL_BUFFER_SIZE);
		FSFILE_Write(cia, &written, offset, buffer, INSTALL_BUFFER_SIZE, FS_WRITE_FLUSH);
		offset += read;

		// idk whai this no worcc
		printf("\x1b[29;0HPercent: %lli%% (%luMiB/%lliMiB)", (offset / size) * 100, offset / 1024 / 1024, size / 1024 / 1024);

	}
	while(offset < size);
	free(buffer);

	res = AM_FinishCiaInstall(cia);
	if (R_FAILED(res)) {
		print_error("Failed finishing the cia install", res);
		pause_3ds();
		return -1;
	}

	res = FSFILE_Close(file);
	if (R_FAILED(res)) {
		print_error("Failed closing file handle", res);
		pause_3ds();
		return -1;
	}

	formatted_print("Finished installation of cia", 0, 29);
	pause_3ds();

	return 0;

}

// https://github.com/Universal-Team/Universal-Updater/blob/master/source/utils/cia.cpp#L55
// Fucking bitwise
FS_MediaType getTitleDestination(u64 titleId)
{

	u16 platform = (u16) ((titleId >> 48) & 0xFFFF);
	u16 category = (u16) ((titleId >> 32) & 0xFFFF);
	u8 variation = (u8) (titleId & 0xFF);

	FS_MediaType type = platform == 0x0003
						|| (platform == 0x0004 && ((category & 0x8011) != 0
						|| (category == 0x0000 && variation == 0x02)))
						? MEDIATYPE_NAND
						: MEDIATYPE_SD;

	return type;

}
