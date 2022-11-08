#ifndef NES_EMU5_BUS_H
#define NES_EMU5_BUS_H


#include "Catridge.h"
#include "PPU.h"


uint8_t BUS_cpuRead(uint16_t addr);
void BUS_cpuWrite(uint16_t addr, uint8_t src);
void dma_clock();
extern uint8_t dma_transfer;
extern uint8_t controller_state[];


#endif //NES_EMU5_BUS_H
