//
// Created by boris on 17/10/2020.
//

#include "BUS.h"
#include "spi.h"

uint8_t cpuRam[2*1024] = {0};
uint8_t audio[32];

uint8_t controller_state[2];
uint8_t dma_page = 0;
uint8_t dma_addr = 0;
uint8_t dma_transfer = 0;

uint8_t debug_memory[0x10000];

uint8_t BUS_cpuRead(uint16_t addr) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        return cpuRam[addr & 0x07FF];
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        return PPU_cpuRead(addr & 0x0007);
    } else if (addr >= 0x4016 && addr <= 0x4017) {
        uint8_t data = (controller_state[addr & 0x0001] & 0x80) > 0;
        controller_state[addr & 0x0001] <<= 1;
        return data;
    } else if (addr >= 0x8000 && addr <= 0xFFFF) {
        return PRG_ROM[addr & 0x7FFF];
    } else {

        return debug_memory[addr];
    }
}

void BUS_cpuWrite(uint16_t addr, uint8_t src) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        cpuRam[addr & 0x07FF] = src;
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        PPU_cpuWrite(addr & 0x0007, src);
    } else if (addr >= 0x4000 && addr <= 0x4013) {
        audio[addr & 0x001F] = src;
    } else if (addr == 0x4014) {
        dma_page = src;
        dma_addr = 0;
        dma_transfer = 1;
    } else if (addr == 0x4015) {

    } else if (addr >= 0x4016 && addr <= 0x4017) {
    	HAL_SPI_Receive(addr & 0x0001 ? &hspi6 : &hspi4, &(controller_state[addr & 0x0001]), 1, 1);
    } else if (addr >= 0x8000 && addr <= 0xFFFF) {
        PRG_ROM[addr & 0x7FFF] = src;
    } else {
        debug_memory[addr] = src;

    }
}


void dma_clock() {
    if (dma_transfer) {
        pOAM[dma_addr] = BUS_cpuRead(((uint16_t)(dma_page) << 8) | dma_addr);
        dma_addr++;
        if (dma_addr == 0) {
            dma_transfer = 0;
        }
    }
}

