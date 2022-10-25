//
// Created by boris on 17/10/2020.
//

#ifndef NES_EMU5_CATRIDGE_H
#define NES_EMU5_CATRIDGE_H

#include "main.h"

extern uint8_t PRG_ROM[];
extern uint8_t CHR_ROM[];

void InitFlash();
uint8_t isNesFile(const char* name);
void LoadCartridge(const char* filename);

void FindCartridges();
void LoadSelectedCatrirdge();

extern uint8_t cartridgeIsLoaded;


#endif //NES_EMU5_CATRIDGE_H
