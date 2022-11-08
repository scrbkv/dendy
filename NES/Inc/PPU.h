//
// Created by boris on 17/10/2020.
//

#ifndef NES_EMU5_PPU_H
#define NES_EMU5_PPU_H

#include "Catridge.h"



void PPU_clock();
void PPU_reset();

uint8_t PPU_read(uint16_t addr);
void PPU_write(uint16_t addr, uint8_t src);

uint8_t PPU_cpuRead(uint16_t addr);
void PPU_cpuWrite(uint16_t addr, uint8_t src);

extern uint8_t nmi;

extern uint8_t pOAM[256];
extern uint8_t sOAM[32];

#endif //NES_EMU5_PPU_H
