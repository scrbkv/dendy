//
// Created by boris on 29/09/2020.
//

#ifndef NES_EMU5_CPU_H
#define NES_EMU5_CPU_H

#include "BUS.h"
#include "main.h"

    void CPU_clock();
    void CPU_reset();
    void CPU_IRQ();
    void CPU_NMI();

    extern uint8_t debug_flag;

#endif //NES_EMU5_CPU_H
