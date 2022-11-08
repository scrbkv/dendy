//
// Created by boris on 29/09/2020.
//

#include "CPU.h"


enum FLAGS {
	C = (1 << 0),	// Carry Bit
	Z = (1 << 1),	// Zero
	I = (1 << 2),	// Disable Interrupts
	D = (1 << 3),	// Decimal Mode (unused in this implementation)
	B = (1 << 4),	// Break
	U = (1 << 5),	// Unused
	V = (1 << 6),	// Overflow
	N = (1 << 7),	// Negative
};


static uint8_t read(uint16_t addr);
static void write(uint16_t addr, uint8_t src);

static uint8_t getFlag(uint8_t p);
static void setFlag(uint8_t p, uint16_t a);

static int cycle;

static uint8_t opcode;
static uint8_t fetched;
static uint16_t addr_abs;
static uint16_t addr_rel;

static void fetch();

static uint8_t a;
static uint8_t y;
static uint8_t x;
static uint16_t pc;
static uint8_t status;
static uint8_t s;

uint8_t IMP();	uint8_t IMM();
uint8_t ZP0();	uint8_t ZPX();
uint8_t ZPY();	uint8_t REL();
uint8_t ABS();	uint8_t ABX();
uint8_t ABY();	uint8_t IND();
uint8_t IZX();	uint8_t IZY();


uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

uint8_t XXX();



typedef struct
{
	uint8_t     (*operate )(void);
	uint8_t     (*addrmode)(void);
	uint8_t     cycles;
} INSTRUCTION;

uint8_t ram[0xFFFF + 1];

INSTRUCTION lookup[256] = {
                    {BRK, IMP, 7},
                    {ORA, IZX, 6},
                    {XXX, IMP, 2},
                    {XXX, IZX, 8},
                    {NOP, ZP0, 3},
                    {ORA, ZP0, 3},
                    {ASL, ZP0, 5},
                    {XXX, ZP0, 5},

                    {PHP, IMP, 3},
                    {ORA, IMM, 2},
                    { ASL, IMP, 2},
                    { XXX, IMM, 2},
                    { NOP, ABS, 4},
                    { ORA, ABS, 4},
                    { ASL, ABS, 6},
                    {XXX, ABS, 6},

                    {BPL, REL, 2},
                    {ORA, IZY, 5},
                    {XXX, IMP, 2},
                    {XXX, IZY, 8},
                    {NOP, ZPX, 4},
                    {ORA, ZPX, 4},
                    {ASL, ZPX, 6},
                    {XXX, IMP, 6},

                    {CLC, IMP, 2},
                    {ORA, ABY, 4},
                    { NOP, IMP, 2},
                    {XXX, IMP, 7},
                    { NOP, IMP, 4},
                    { ORA, ABX, 4},
                    { ASL, ABX, 7},
                    { XXX, IMP, 7},

                    { JSR, ABS, 6},
                    { AND, IZX, 6},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { BIT, ZP0, 3},
                    { AND, ZP0, 3},
                    { ROL, ZP0, 5},
                    { XXX, IMP, 5},

                    { PLP, IMP, 4},
                    { AND, IMM, 2},
                    { ROL, IMP, 2},
                    { XXX, IMP, 2},
                    { BIT, ABS, 4},
                    { AND, ABS, 4},
                    { ROL, ABS, 6},
                    { XXX, IMP, 6},

                    { BMI, REL, 2},
                    { AND, IZY, 5},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { NOP, IMP, 4},
                    { AND, ZPX, 4},
                    {ROL, ZPX, 6},
                    { XXX, IMP, 6},

                    { SEC, IMP, 2},
                    { AND, ABY, 4},
                    { NOP, IMP, 2},
                    { XXX, IMP, 7},
                    { NOP, IMP, 4},
                    { AND, ABX, 4},
                    { ROL, ABX, 7},
                    { XXX, IMP, 7},

                    { RTI, IMP, 6},
                    { EOR, IZX, 6},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { NOP, IMP, 3},
                    { EOR, ZP0, 3},
                    { LSR, ZP0, 5},
                    { XXX, IMP, 5},

                    { PHA, IMP, 3},
                    { EOR, IMM, 2},
                    { LSR, IMP, 2},
                    { XXX, IMP, 2},
                    { JMP, ABS, 3},
                    { EOR, ABS, 4},
                    { LSR, ABS, 6},
                    { XXX, IMP, 6},

                    { BVC, REL, 2},
                    { EOR, IZY, 5},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { NOP, IMP, 4},
                    { EOR, ZPX, 4},
                    { LSR, ZPX, 6},
                    { XXX, IMP, 6},

                    { CLI, IMP, 2},
                    { EOR, ABY, 4},
                    { NOP, IMP, 2},
                    { XXX, IMP, 7},
                    { NOP, IMP, 4},
                    { EOR, ABX, 4},
                    {LSR, ABX, 7},
                    { XXX, IMP, 7},

                    { RTS, IMP, 6},
                    { ADC, IZX, 6},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { NOP, IMP, 3},
                    { ADC, ZP0, 3},
                    { ROR, ZP0, 5},
                    { XXX, IMP, 5},

                    { PLA, IMP, 4},
                    { ADC, IMM, 2},
                    { ROR, IMP, 2},
                    { XXX, IMP, 2},
                    { JMP, IND, 5},
                    { ADC, ABS, 4},
                    { ROR, ABS, 6},
                    { XXX, IMP, 6},

                    { BVS, REL, 2},
                    { ADC, IZY, 5},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { NOP, IMP, 4},
                    { ADC, ZPX, 4},
                    { ROR, ZPX, 6},
                    { XXX, IMP, 6},

                    {SEI, IMP, 2},
                    { ADC, ABY, 4},
                    { NOP, IMP, 2},
                    { XXX, IMP, 7},
                    { NOP, IMP, 4},
                    { ADC, ABX, 4},
                    { ROR, ABX, 7},
                    { XXX, IMP, 7},

                    { NOP, IMP, 2},
                    { STA, IZX, 6},
                    { NOP, IMP, 2},
                    { XXX, IMP, 6},
                    { STY, ZP0, 3},
                    { STA, ZP0, 3},
                    { STX, ZP0, 3},
                    { XXX, IMP, 3},

                    { DEY, IMP, 2},
                    { NOP, IMP, 2},
                    { TXA, IMP, 2},
                    { XXX, IMP, 2},
                    { STY, ABS, 4},
                    { STA, ABS, 4},
                    { STX, ABS, 4},
                    { XXX, IMP, 4},

                    { BCC, REL, 2},
                    { STA, IZY, 6},
                    { XXX, IMP, 2},
                    { XXX, IMP, 6},
                    { STY, ZPX, 4},
                    { STA, ZPX, 4},
                    { STX, ZPY, 4},
                    { XXX, IMP, 4},

                    { TYA, IMP, 2},
                    { STA, ABY, 5},
                    { TXS, IMP, 2},
                    { XXX, IMP, 5},
                    { NOP, IMP, 5},
                    { STA, ABX, 5},
                    { XXX, IMP, 5},
                    { XXX, IMP, 5},

                    { LDY, IMM, 2},
                    { LDA, IZX, 6},
                    { LDX, IMM, 2},
                    { XXX, IMP, 6},
                    { LDY, ZP0, 3},
                    { LDA, ZP0, 3},
                    { LDX, ZP0, 3},
                    { XXX, IMP, 3},

                    { TAY, IMP, 2},
                    { LDA, IMM, 2},
                    { TAX, IMP, 2},
                    { XXX, IMP, 2},
                    { LDY, ABS, 4},
                    { LDA, ABS, 4},
                    { LDX, ABS, 4},
                    { XXX, IMP, 4},

                    { BCS, REL, 2},
                    { LDA, IZY, 5},
                    { XXX, IMP, 2},
                    { XXX, IMP, 5},
                    { LDY, ZPX, 4},
                    { LDA, ZPX, 4},
                    { LDX, ZPY, 4},
                    { XXX, IMP, 4},

                    { CLV, IMP, 2},
                    { LDA, ABY, 4},
                    { TSX, IMP, 2},
                    { XXX, IMP, 4},
                    { LDY, ABX, 4},
                    { LDA, ABX, 4},
                    { LDX, ABY, 4},
                    { XXX, IMP, 4},

                    { CPY, IMM, 2},
                    { CMP, IZX, 6},
                    { NOP, IMP, 2},
                    { XXX, IMP, 8},
                    { CPY, ZP0, 3},
                    { CMP, ZP0, 3},
                    { DEC, ZP0, 5},
                    { XXX, IMP, 5},

                    { INY, IMP, 2},
                    { CMP, IMM, 2},
                    { DEX, IMP, 2},
                    { XXX, IMP, 2},
                    { CPY, ABS, 4},
                    { CMP, ABS, 4},
                    { DEC, ABS, 6},
                    { XXX, IMP, 6},

                    { BNE, REL, 2},
                    { CMP, IZY, 5},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { NOP, IMP, 4},
                    { CMP, ZPX, 4},
                    { DEC, ZPX, 6},
                    { XXX, IMP, 6},

                    { CLD, IMP, 2},
                    { CMP, ABY, 4},
                    { NOP, IMP, 2},
                    { XXX, IMP, 7},
                    { NOP, IMP, 4},
                    { CMP, ABX, 4},
                    { DEC, ABX, 7},
                    { XXX, IMP, 7},

                    { CPX, IMM, 2},
                    { SBC, IZX, 6},
                    { NOP, IMP, 2},
                    { XXX, IMP, 8},
                    { CPX, ZP0, 3},
                    { SBC, ZP0, 3},
                    { INC, ZP0, 5},
                    { XXX, IMP, 5},

                    { INX, IMP, 2},
                    { SBC, IMM, 2},
                    { NOP, IMP, 2},
                    { SBC, IMP, 2},
                    { CPX, ABS, 4},
                    { SBC, ABS, 4},
                    { INC, ABS, 6},
                    { XXX, IMP, 6},

                    { BEQ, REL, 2},
                    { SBC, IZY, 5},
                    { XXX, IMP, 2},
                    { XXX, IMP, 8},
                    { NOP, IMP, 4},
                    { SBC, ZPX, 4},
                    { INC, ZPX, 6},
                    { XXX, IMP, 6},

                    { SED, IMP, 2},
                    { SBC, ABY, 4},
                    { NOP, IMP, 2},
                    { XXX, IMP, 7},
                    { NOP, IMP, 4},
                    { SBC, ABX, 4},
                    { INC, ABX, 7},
                    { XXX, IMP, 7},
            };


uint8_t IMP() {
    fetched = a;
    return 0;
}

uint8_t IMM() {
    addr_abs = pc;
    pc++;
    return 0;
}

uint8_t ZP0() {
    addr_abs = read(pc);
    pc++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t ZPX() {
    addr_abs = read(pc) + x;
    pc++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t ZPY() {
    addr_abs = read(pc) + y;
    pc++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t REL() {
    uint8_t tmp = read(pc);
    pc++;
    addr_rel = (uint16_t)((int8_t)(tmp));

    return 0;
}

uint8_t ABS() {

    uint16_t lo = read(pc);
    pc++;

    uint16_t hi = read(pc);
    pc++;
    addr_abs = (hi << 8) | lo;

    return 0;
}

uint8_t ABX() {
    uint16_t lo = read(pc);
    pc++;
    uint16_t hi = read(pc);
    pc++;
    addr_abs = (hi << 8) | lo;
    addr_abs += (uint16_t)(x);
    return 0;
}

uint8_t ABY() {
    uint16_t lo = read(pc);
    pc++;
    uint16_t hi = read(pc);
    pc++;
    addr_abs = (hi << 8) | lo;
    addr_abs += (uint16_t)(y);
    return 0;
}

uint8_t IND() {
    uint16_t ptr_lo = read(pc);
    pc++;
    uint16_t ptr_hi = read(pc);
    pc++;

    uint16_t ptr = (ptr_hi << 8) | ptr_lo;

    if (ptr_lo == 0x00FF) // Simulate page boundary hardware bug
    {
        addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
    }
    else // Behave normally
    {
        addr_abs = ((uint16_t)(read(ptr + 1))<< 8) | read(ptr + 0);
    }

    return 0;
}

uint8_t IZX() {
    uint16_t t = read(pc);
    pc++;

    uint16_t lo = read((t + x) & 0x00FF);
    uint16_t hi = read((t + x + 1) & 0x00FF);

    addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t IZY() {
    uint16_t t = read(pc);
    pc++;

    uint16_t lo = read(t & 0x00FF);
    uint16_t hi = read((t + 1) & 0x00FF);

    addr_abs = (hi << 8) | lo;
    addr_abs += y;
    return 0;
}


uint8_t ADC() {
    fetch();
    uint16_t tmp = (uint16_t)(a) + (uint16_t)(fetched) + (uint16_t)(getFlag(C));

    setFlag(C, tmp & 0x0100);
    setFlag(Z, (tmp & 0x00FF) == 0);
    setFlag(N, tmp & 0x0080);
    setFlag(V, (a ^ tmp) & (fetched ^ tmp) & 0x0080);

    a = tmp & 0x00FF;

    return 0;
}

uint8_t AND() {
    fetch();
    a = a & fetched;

    setFlag(Z, a == 0);
    setFlag(N, a & 0x80);

    return 0;
}

uint8_t ASL() {
    fetch();

    uint16_t tmp = (uint16_t) a << 1;

    setFlag(C, tmp & 0x0100);
    setFlag(Z, (tmp & 0x00FF) == 0);
    setFlag(N, tmp & 0x0080);

    if (lookup[opcode].addrmode == &IMP)
        a = tmp & 0x00FF;
    else
        write(addr_abs, tmp & 0x00FF);
    return 0;
}

uint8_t BCS() {
    if (getFlag(C)) {
        cycle++;
        addr_abs = pc + addr_rel;

        if ((addr_abs & 0xFF00) != (pc & 0xFF00))
            cycle++;

        pc = pc + addr_rel;

    }
    return 0;
}

uint8_t BEQ() {
    if (getFlag(Z)) {
        cycle++;
        addr_abs = pc + addr_rel;

        if ((addr_abs & 0xFF00) != (pc & 0xFF00))
            cycle++;

        pc = pc + addr_rel;
    }
    return 0;
}

uint8_t BIT() {
    fetch();
    uint8_t tmp = a & fetched;
    setFlag(Z, tmp == 0);
    setFlag(V, fetched & 0x40);
    setFlag(N, fetched & 0x80);


    return 0;
}

uint8_t BMI() {
    if (getFlag(N)) {
        cycle++;
        addr_abs = pc + addr_rel;

        if ((addr_abs & 0xFF00) != (pc & 0xFF00))
            cycle++;
        pc = pc + addr_rel;
    }
    return 0;
}

uint8_t BNE() {
    if (getFlag(Z) == 0) {
        cycle++;
        addr_abs = pc + addr_rel;

        if ((addr_abs & 0xFF00) != (pc & 0xFF00))
            cycle++;

        pc = pc + addr_rel;
    }
    return 0;
}

uint8_t BPL() {
    if (getFlag(N) == 0) {
        cycle++;
        addr_abs = pc + addr_rel;

        if ((addr_abs & 0xFF00) != (pc & 0xFF00))
            cycle++;
        pc = pc + addr_rel;
    }
    return 0;
}

uint8_t BRK() {
    pc++;
    setFlag(I, 1);

    write(0x0100 + s, (pc >> 8) & 0x00FF);
    s--;
    write(0x0100 + s, pc & 0x00FF);
    s--;
    setFlag(B, 1);
    write(0x0100 + s, status);
    s--;
    setFlag(B, 0);

    uint8_t al = read(0xFFFE);
    uint8_t ah = read(0xFFFF);
    pc = ((uint16_t)(ah) << 8) | (uint16_t)(al);
    return 0;
}

uint8_t BVC() {
    if (getFlag(V) == 0) {
        cycle++;
        addr_abs = pc + addr_rel;
        pc = pc + addr_rel;
    }
    return 0;
}

uint8_t BVS() {
    if (getFlag(V) == 1) {
        cycle++;
        addr_abs = pc + addr_rel;
        pc = pc + addr_rel;
    }
    return 0;
}

uint8_t CLC() {
    setFlag(C, 0);
    return 0;
}

uint8_t CLD() {
    setFlag(D, 0);
    return 0;
}

uint8_t CLI() {
    setFlag(I, 0);
    return 0;
}

uint8_t CLV() {
    setFlag(V, 0);
    return 0;
}

uint8_t CMP() {
    fetch();
    uint16_t tmp = (uint16_t)(a) - (uint16_t)(fetched);
    setFlag(N, tmp & 0x0080);
    setFlag(Z, (tmp & 0x00FF) == 0);
    setFlag(C, a >= fetched);
    return 0;
}

uint8_t CPX() {
    fetch();
    uint16_t tmp = (uint16_t)(x) - (uint16_t)(fetched);
    setFlag(N, tmp & 0x0080);
    setFlag(Z, (tmp & 0x00FF) == 0);
    setFlag(C, x >= fetched);
    return 0;
}

uint8_t CPY() {
    fetch();
    uint16_t tmp = (uint16_t)(y) - (uint16_t)(fetched);
    setFlag(N, tmp & 0x0080);
    setFlag(Z, (tmp & 0x00FF) == 0);
    setFlag(C, y >= fetched);

    return 0;
}

uint8_t DEC() {
    fetch();
    uint8_t tmp = fetched - 1;
    setFlag(N, tmp & 0x80);
    setFlag(Z, tmp == 0);
    write(addr_abs, tmp);
    return 0;
}

uint8_t DEX() {
    x--;
    setFlag(N, x & 0x80);
    setFlag(Z, x == 0);

    return 0;
}

uint8_t DEY() {
    y--;
    setFlag(N, y & 0x80);
    setFlag(Z, y == 0);
    return 0;
}

uint8_t EOR() {
    fetch();
    a = a ^ fetched;
    setFlag(N, a & 0x80);
    setFlag(Z, a == 0);
    return 0;
}

uint8_t INC() {
    fetch();
    uint8_t tmp = fetched + 1;
    setFlag(N, tmp & 0x80);
    setFlag(Z, tmp == 0);
    write(addr_abs, tmp);
    return 0;
}

uint8_t INX() {
    x++;
    setFlag(N, x & 0x80);
    setFlag(Z, x == 0);

    return 0;
}

uint8_t INY() {
    y++;
    setFlag(N, y & 0x80);
    setFlag(Z, y == 0);
    return 0;
}

uint8_t JMP() {
    pc = addr_abs;
    return 0;
}

uint8_t JSR() {
    pc--;

    write(0x0100 + s, (pc >> 8) & 0x00FF);
    s--;
    write(0x0100 + s, pc & 0x00FF);
    s--;

    pc = addr_abs;
    return 0;
}

uint8_t LDA() {
    fetch();
    a = fetched;
    setFlag(N, a & 0x80);
    setFlag(Z, a == 0);
    return 0;
}

uint8_t LDX() {
    //std::cout<< std::hex <<"LDX_address: " << int(addr_abs) << std::endl;

    fetch();
    x = fetched;
    setFlag(N, x & 0x80);
    setFlag(Z, x == 0);
    return 0;
}

uint8_t LDY() {
    //std::cout<< std::hex <<"LDY_address: " << int(addr_abs) << std::endl;

    fetch();
    y = fetched;
    setFlag(N, y & 0x80);
    setFlag(Z, y == 0);
    return 0;
}

uint8_t LSR() {
    fetch();
    uint8_t tmp = fetched >> 1;
    setFlag(C, fetched & 0x01);
    setFlag(N, tmp & 0x80);
    setFlag(Z, tmp == 0);
    if (lookup[opcode].addrmode == &IMP)
        a = tmp & 0x00FF;
    else
        write(addr_abs, tmp & 0x00FF);
    return 0;
}

uint8_t NOP() {
    return 0;
}

uint8_t ORA() {
    fetch();
    a = a | fetched;

    setFlag(Z, a == 0);
    setFlag(N, a & 0x80);
    return 0;
}

uint8_t PHA() {
    write(0x0100 + s, a);
    s--;
    return 0;
}

uint8_t PHP() {
    write(0x0100 + s, status | B | U);
    setFlag(B, 0); //Flag test error with it;
    setFlag(U, 0);
    s--;

    return 0;
}


uint8_t PLA() {
    s++;
    a = read(0x0100 + s);
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
    return 0;
}

uint8_t PLP() {
    s++;
    status = read(0x0100 + s);
    setFlag(U, 1);
    return 0;
}

uint8_t ROL() {
    fetch();
    //std::cout << "ROL fetched: " << std::hex << (uint16_t)fetched << std::endl;
    uint16_t tmp = ((uint16_t)(fetched) << 1) | (uint16_t)(getFlag(C));
    setFlag(C, tmp & 0xFF00);
    setFlag(N, tmp & 0x0080);
    setFlag(Z, (tmp & 0x00FF) == 0);

    if (lookup[opcode].addrmode == &IMP)
        a = tmp & 0x00FF;
    else
        write(addr_abs, tmp & 0x00FF);
    return 0;
}

uint8_t ROR() {
    fetch();
    uint8_t tmp = ((uint8_t)(getFlag(C)) << 7) | (fetched >> 1);
    setFlag(C, fetched & 0x0001);
    setFlag(N, tmp & 0x0080);
    setFlag(Z, (tmp & 0x00FF) == 0);
    if (lookup[opcode].addrmode == &IMP)
        a = tmp & 0x00FF;
    else
        write(addr_abs, tmp & 0x00FF);

    return 0;
}

uint8_t RTI() {
    s++;
    status = read(0x0100 + s);
    setFlag(I, 1);
    setFlag(B, 1);

    s++;
    pc = (uint16_t)(read(0x0100 + s));
    s++;
    pc |= read(0x0100 + s) << 8;

    return 0;
}

uint8_t RTS() {
    s++;
    pc = (uint16_t)(read(0x0100 + s));
    s++;
    pc |= (uint16_t)(read(0x0100 + s)) << 8;

    pc++;
    return 0;
}

uint8_t SBC() {
    fetch();

    uint16_t value = ((uint16_t) fetched) ^0x00FF;

    uint16_t tmp = (uint16_t) a + value + (uint16_t) getFlag(C);

    setFlag(C, tmp & 0xFF00);
    setFlag(Z, (tmp & 0x00FF) == 0);
    setFlag(N, tmp & 0x80);
    setFlag(V, (tmp ^ (uint16_t) a) & (tmp ^ value) & 0x0080);

    a = tmp & 0x00FF;

    return 0;
}

uint8_t SEC() {
    setFlag(C, 1);
    return 0;
}

uint8_t SED() {
    setFlag(D, 1);
    return 0;
}

uint8_t SEI() {
    setFlag(I, 1);
    return 0;
}

uint8_t STA() {
    //std::cout<< std::hex <<"STA_address: " << int(addr_abs) << std::endl;
    write(addr_abs, a);
    return 0;
}

uint8_t STX() {
    //std::cout<< std::hex <<"STX_address: " << int(addr_abs) << std::endl;
    write(addr_abs, x);
    return 0;
}

uint8_t STY() {
    //std::cout<< std::hex <<"STY_address: " << int(addr_abs) << std::endl;
    write(addr_abs, y);
    return 0;
}

uint8_t TAX() {
    x = a;

    setFlag(N, x & 0x80);
    setFlag(Z, x == 0);
    return 0;
}

uint8_t TAY() {
    y = a;

    setFlag(N, y & 0x80);
    setFlag(Z, y == 0);
    return 0;
}

uint8_t TSX() {
    x = s;

    setFlag(N, x & 0x80);
    setFlag(Z, x == 0);
    return 0;
}

uint8_t TXA() {
    a = x;

    setFlag(N, a & 0x80);
    setFlag(Z, a == 0);
    return 0;
}

uint8_t TXS() {
    s = x;

    //setFlag(N, s & 0x80);
    //setFlag(Z, s == 0);
    return 0;
}

uint8_t TYA() {
    a = y;

    setFlag(N, a & 0x80);
    setFlag(Z, a == 0);
    return 0;
}

uint8_t BCC() {
    if (getFlag(C) == 0) {
        cycle++;
        addr_abs = pc + addr_rel;
        if ((addr_abs & 0xFF00) != (pc & 0xFF00))
            cycle++;

        pc = pc + addr_rel;


    }
    return 0;
}

uint8_t XXX() {
    return 0;
}

static uint8_t getFlag(uint8_t p) {
    return ((status & p) > 0) ? 1 : 0;
}

static void setFlag(uint8_t p, uint16_t a) {
    a ? (status |= p) : (status &= ~p);
}

static uint8_t read(uint16_t addr) {

    return BUS_cpuRead(addr);
    //return ram[addr];
}

static void write(uint16_t addr, uint8_t src) {
    BUS_cpuWrite(addr, src);
    //ram[addr] = src;
}

void CPU_reset() {
    addr_abs = 0xFFFC;
    uint16_t lo = read(addr_abs + 0);
    uint16_t hi = read(addr_abs + 1);

    // Set it
    pc = (hi << 8) | lo;
    // Reset internal registers
    a = 0;
    x = 0;
    y = 0;
    s = 0xFD;
    status = 0x00 | U;

    // Clear internal helper variables
    addr_rel = 0x0000;
    addr_abs = 0x0000;
    fetched = 0x00;

    cycle = 8;
}

static void fetch() {
    if (lookup[opcode].addrmode != &IMP){
        fetched = read(addr_abs);
    }

}


void CPU_IRQ() {
    if (getFlag(I) == 0) {
        write(0x0100 + s, (pc >> 8) & 0x00FF);
        s--;
        write(0x0100 + s, pc & 0x00FF);
        s--;
        write(0x0100 + s, status);
        s--;
        uint8_t al = read(0xFFFA);
        uint8_t ah = read(0xFFFB);
        pc = ((uint16_t)(ah) << 8) | (uint16_t)(al);
        setFlag(I, 1);
        setFlag(B, 0);
        setFlag(U, 1);

        cycle = 8;
    }
}

void CPU_NMI() {
    write(0x0100 + s, (pc >> 8) & 0x00FF);
    s--;
    write(0x0100 + s, pc & 0x00FF);
    s--;
    write(0x0100 + s, status);
    s--;
    uint8_t al = read(0xFFFA);
    uint8_t ah = read(0xFFFB);
    pc = ((uint16_t)(ah) << 8) | (uint16_t)(al);
    setFlag(I, 1);
    setFlag(B, 0);
    setFlag(U, 1);


    cycle = 8;
}

void CPU_clock() {
    if (cycle == 0) {

        opcode = read(pc);

        pc++;

        (*lookup[opcode].addrmode)();
        (*lookup[opcode].operate)();
        cycle = lookup[opcode].cycles;
    }
    cycle--;
}

