//
// Created by boris on 17/10/2020.
//

#include "Catridge.h"

#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"
#include "fmc.h"

#include "CPU.h"
#include "PPU.h"
#include "BUS.h"

#include "lcd.h"
#include "w25qxx_qspi.h"

uint8_t PRG_ROM[32*1024*4];
uint8_t CHR_ROM[8*1024*4];

char* cartridges;
int cartridgesCount = 0;
int selectedCartridge = -1;

uint8_t cartridgeIsLoaded = 0;


void InitFlash() {
	w25qxx_Init();
	w25qxx_EnterQPI();
}

uint8_t isNesFile(const char* name) {
	uint16_t len = strlen(name);
	while (name[len] != '.' && len > 0)
		--len;
	++len;

	return (name[len] == 'n' && name[len+1] == 'e' && name[len+2] == 's' && name[len+3] == 0);
}

void LoadCartridge(const char* filename) {
	FATFS fs;
	f_mount(&fs, "", 0);
	DIR dir;
	f_opendir(&dir, "/");
	FIL fp;
	FRESULT res = f_open(&fp, filename, FA_READ);
	if (res != FR_OK) {
		f_closedir (&dir);
		f_mount(NULL, "", 0);
		return;
	}

	uint32_t ines_constant = 0;
	uint read = 0;
	res = f_read (&fp, &ines_constant, 4, &read);
	if (res != FR_OK || read != 4) {
		f_close (&fp);
		f_closedir (&dir);
		f_mount(NULL, "", 0);
		return;
	}

	uint16_t prg_rom_size = 0;
	res = f_read (&fp, &prg_rom_size, 1, &read);
	if (res != FR_OK || read != 1) {
		f_close (&fp);
		f_closedir (&dir);
		f_mount(NULL, "", 0);
		return;
	}

	uint16_t chr_rom_size = 0;
	res = f_read (&fp, &chr_rom_size, 1, &read);
	if (res != FR_OK || read != 1) {
		f_close (&fp);
		f_closedir (&dir);
		f_mount(NULL, "", 0);
		return;
	}

	prg_rom_size *= 16384;
	chr_rom_size *= 8192;

	res = f_lseek (&fp, 16);
	if (res != FR_OK) {
		f_close (&fp);
		f_closedir (&dir);
		f_mount(NULL, "", 0);
		return;
	}

	res = f_read(&fp, PRG_ROM + (0x8000 - prg_rom_size), prg_rom_size, &read);
	if (res != FR_OK || read != prg_rom_size) {
		f_close (&fp);
		f_closedir (&dir);
		f_mount(NULL, "", 0);
		return;
	}

	res = f_read(&fp, CHR_ROM + (8192 - chr_rom_size), chr_rom_size, &read);
	if (res != FR_OK || read != chr_rom_size) {
		f_close (&fp);
		f_closedir (&dir);
		f_mount(NULL, "", 0);
		return;
	}

	f_close (&fp);
	f_closedir (&dir);
	f_mount(NULL, "", 0);
	cartridgeIsLoaded = 1;
}



void FindCartridges() {
	FATFS fs;
	f_mount(&fs, "", 0);
	DIR dir;
	f_opendir(&dir, "/");
	FILINFO fileInfo;

	FRESULT res = f_readdir(&dir, &fileInfo);
	while ((res == FR_OK) && (fileInfo.fname[0] != '\0')) {
		if (!(fileInfo.fattrib & AM_DIR) && isNesFile(fileInfo.fname))
			++cartridgesCount;
		res = f_readdir(&dir, &fileInfo);
	}
	f_closedir (&dir);
	if (cartridgesCount == 0)
		return;

	cartridges = malloc(cartridgesCount * 256);
	memset(cartridges, 0, cartridgesCount * 256);

	f_opendir(&dir, "/");

	char* cartridges2 = cartridges;
	res = f_readdir(&dir, &fileInfo);
	while ((res == FR_OK) && (fileInfo.fname[0] != '\0')) {
		if (!(fileInfo.fattrib & AM_DIR) && isNesFile(fileInfo.fname)) {
			memcpy(cartridges2, fileInfo.fname, strlen(fileInfo.fname));
			cartridges2 += 256;
		}
		res = f_readdir(&dir, &fileInfo);
	}
	selectedCartridge = 0;
	f_closedir (&dir);
	f_mount(NULL, "", 0);
}

void LoadSelectedCatrirdge() {
	if (selectedCartridge == -1)
		return;

	LoadCartridge(cartridges + 256 * selectedCartridge);
}
