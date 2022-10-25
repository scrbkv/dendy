//
// Created by boris on 17/10/2020.
//

#include "PPU.h"
#include "main.h"
#include "cmsis_os2.h"

__IO uint8_t* DATA = (uint8_t*)0xC0080000;


static const uint16_t CHR_ROM_OFFSET[] = {0x0000, 0x1000};
static const uint16_t VRAM_OFFSET = 0x2000;
static const uint16_t VRAMA_OFFSET = 0x23C0;

uint8_t nmi = 0;
uint8_t pOAM[256] = {};
uint8_t sOAM[32];

union
{
    struct
    {
        uint8_t unused : 5;
        uint8_t sprite_overflow : 1;
        uint8_t sprite_zero_hit : 1;
        uint8_t vertical_blank : 1;
    };

    uint8_t reg;
} PPU_status;

void loadBackgroundShifters();
    void updateShifters();

    void incrementalScrollX();
    void incrementalScrollY();
    void transferAddressX();
    void transferAddressY();
    void render();


    int cycle = 0;
    int scanline = 0;
    uint64_t frame = 0;

    uint8_t bg_next_tile_id = 0;
    uint8_t bg_next_at = 0;
    uint8_t bg_next_at_tile = 0;
    uint8_t bg_next_tile_lsb = 0;
    uint8_t bg_next_tile_msb = 0;

    uint16_t bg_shifter_pattern_lo = 0;
    uint16_t bg_shifter_patter_hi = 0;
    uint16_t bg_shifter_at_lo = 0;
    uint16_t bg_shifter_at_hi = 0;

    uint8_t sprite_count = 0;
    uint8_t sp_shifter_pattern_lo[8];
    uint8_t sp_shifter_pattern_hi[8];

    uint8_t spriteZeroHitPosible = 0;
    uint8_t spriteZeroHitBegin = 0;
    uint8_t color = 0;

    uint8_t VRAM[0x1000];
    uint8_t palette[32];


    typedef union  {
        struct {
            uint16_t coarse_x: 5;
            uint16_t coarse_y: 5;
            uint16_t nametable_x: 1;
            uint16_t nametable_y: 1;
            uint16_t fine_y: 3;
            uint16_t unused: 1;
        };
        uint16_t reg;
    } vram_t;
    vram_t v_vram;
    vram_t t_vram;
    uint8_t fine_x;

    uint8_t oam_addr = 0;



    union
    {
        struct
        {
            uint8_t grayscale : 1;
            uint8_t render_background_left : 1;
            uint8_t render_sprites_left : 1;
            uint8_t render_background : 1;
            uint8_t render_sprites : 1;
            uint8_t enhance_red : 1;
            uint8_t enhance_green : 1;
            uint8_t enhance_blue : 1;
        };

        uint8_t reg;
    } mask;

    union
    {
        struct
        {
            uint8_t nametable_x : 1;
            uint8_t nametable_y : 1;
            uint8_t increment_mode : 1;
            uint8_t pattern_sprite : 1;
            uint8_t pattern_background : 1;
            uint8_t sprite_size : 1;
            uint8_t slave_mode : 1; // unused
            uint8_t enable_nmi : 1;
        };

        uint8_t reg;
    } control;

    uint8_t address_latch = 0;
    uint8_t ppuDataBuffer = 0;

uint16_t ppuColor[64] = {
    		0xAE73, 0xD120, 0x1500, 0x1340, 0x0E88, 0x02A8, 0x00A0, 0x4078,
    		0x6041, 0x2002, 0x8002, 0xE201, 0xEB19, 0x0000, 0x0000, 0x0000,
        	0xF7BD, 0x9D03, 0xDD21, 0x1E80, 0x17B8, 0x0BE0, 0x40D9, 0x61CA,
        	0x808B, 0xA004, 0x4005, 0x8704, 0x1104, 0x0000, 0x0000, 0x0000,
        	0xFFFF, 0xFF3D, 0xBF5C, 0x5FA4, 0xDFF3, 0xF6FB, 0xACFB, 0xA7F9,
        	0xE7F5, 0x8286, 0xE94E, 0xD35F, 0x5B07, 0x0000, 0x0000, 0x0000,
        	0xFFFF, 0x7FAF, 0xBFC6, 0x5FD6, 0x3FFE, 0x3BFE, 0xF6FD, 0xD5FE,
        	0x34FF, 0xF4E7, 0x97AF, 0xF9B7, 0xFE9F, 0x0000, 0x0000, 0x0000
    };


void PPU_reset() {
    address_latch = 0;
    cycle = 0;
    scanline = 261;
    nmi = 0;
    PPU_status.reg = 0;
    mask.reg = 0;
    control.reg = 0;
    v_vram.reg = 0;
    t_vram.reg = 0;
    fine_x = 0;
    bg_shifter_pattern_lo = 0;
    bg_shifter_patter_hi = 0;
    bg_shifter_at_lo = 0;
    bg_shifter_at_hi = 0;
    bg_next_tile_lsb = 0;
    bg_next_tile_msb = 0;
    bg_next_at_tile = 0;
    oam_addr = 0;
};

void loadBackgroundShifters() {
    bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
    bg_shifter_patter_hi = (bg_shifter_patter_hi & 0xFF00) | bg_next_tile_msb;
    bg_shifter_at_lo = (bg_shifter_at_lo & 0xFF00) | ((bg_next_at_tile & 0x1) ? 0xFF : 0x00);
    bg_shifter_at_hi = (bg_shifter_at_hi & 0xFF00) | ((bg_next_at_tile & 0x2) ? 0xFF : 0x00);
}

void updateShifters() {

    if (mask.render_background) {
        bg_shifter_pattern_lo <<= 1;
        bg_shifter_patter_hi <<= 1;
        bg_shifter_at_lo <<= 1;
        bg_shifter_at_hi <<= 1;
    }
    if (mask.render_sprites && cycle >= 1 && cycle <= 256) {
        for (int i = 0; i < sprite_count; i++) {
            if (sOAM[i * 4 + 3] > 0) {
                sOAM[i * 4 + 3]--;
            } else {
                sp_shifter_pattern_lo[i] <<= 1;
                sp_shifter_pattern_hi[i] <<= 1;
            }
        }
    }
}

void render() {

    uint8_t bg_color = 0;
    uint8_t fg_color = 0;
    uint8_t fg_priority = 0;

    uint8_t bg_zero = 0;
    uint8_t fg_zero = 0;
    if (mask.render_background) {
        uint16_t bit_mux = 0x8000 >> fine_x;
        uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
        uint8_t p1_pixel = (bg_shifter_patter_hi & bit_mux) > 0;
        uint8_t p2_pixel = (bg_shifter_at_lo & bit_mux) > 0;
        uint8_t p3_pixel = (bg_shifter_at_hi & bit_mux) > 0;
        bg_zero = p1_pixel | p0_pixel;
        bg_color = PPU_read(0x3F00 + ((p3_pixel << 3) | (p2_pixel << 2) | (p1_pixel << 1) | (p0_pixel << 0)));
    }

    if (mask.render_sprites) {
        spriteZeroHitBegin = 0;
        for (int i = 0; i < sprite_count; i++) {
            if (sOAM[i * 4 + 3] == 0) {
                uint8_t p0_pixel = (sp_shifter_pattern_lo[i] & 0x80) > 0;
                uint8_t p1_pixel = (sp_shifter_pattern_hi[i] & 0x80) > 0;
                uint8_t fg_palette = (sOAM[i * 4 + 2] & 0x03);
                fg_priority = (sOAM[i * 4 + 2] & 0x20) == 0;

                fg_zero = p1_pixel | p0_pixel;
                fg_color = PPU_read(0x3F10 + ((fg_palette << 2) | (p1_pixel << 1) | (p0_pixel << 0)));
                if (fg_zero != 0) {
                    if (i == 0) {
                        spriteZeroHitBegin = 1;
                    }
                    break;
                }
            }
        }
    }


    if (bg_zero == 0 && fg_zero == 0) {
        color = bg_color;
    } else if (bg_zero == 0 && fg_zero != 0) {
        color = fg_color;
    } else if (bg_zero != 0 && fg_zero == 0) {
        color = bg_color;
    } else {
        if (fg_priority) {
            color = fg_color;
        } else {
            color = bg_color;
        }
        if (spriteZeroHitPosible & spriteZeroHitBegin) {
            if (mask.render_background & mask.render_background) {
                if ((1 <= cycle && cycle <= 256)) {
                    PPU_status.sprite_zero_hit = 1;
                }
            }
        }
    }

}

void incrementalScrollX() {
    if (mask.render_background || mask.render_sprites) {
        if (v_vram.coarse_x == 0x1F) {
            v_vram.coarse_x = 0;
            v_vram.nametable_x = ~v_vram.nametable_x;
        } else {
            v_vram.coarse_x++;
        }
    }
}

void incrementalScrollY() {
    if (mask.render_background || mask.render_sprites) {
        if (v_vram.fine_y != 0x7) {
            v_vram.fine_y++;
        } else {
            v_vram.fine_y = 0;
            if (v_vram.coarse_y == 29) {
                v_vram.coarse_y = 0;
                v_vram.nametable_y ^= 1;
            } else if (v_vram.coarse_y == 31) {
                v_vram.coarse_y = 0;
            } else {
                v_vram.coarse_y++;
            }
        }
    }
}

void transferAddressX() {
    if (mask.render_background || mask.render_sprites) {
        v_vram.coarse_x = t_vram.coarse_x;
        v_vram.nametable_x = t_vram.nametable_x;
    }
}

void transferAddressY() {
    if (mask.render_background || mask.render_sprites) {
        v_vram.coarse_y = t_vram.coarse_y;
        v_vram.nametable_y = t_vram.nametable_y;
        v_vram.fine_y = t_vram.fine_y;
    }
}


uint8_t bitswap(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}


void PPU_clock() {

    uint16_t vram_addr = v_vram.reg & 0x0FFF;

    if ((0 <= scanline && scanline <= 239) || scanline == 261) {
        if ((2 <= cycle && cycle <= 257) || (321 <= cycle && cycle <= 337)) {
            updateShifters();

            switch ((cycle - 1) % 8) {
                case 0:
                    loadBackgroundShifters();
                    bg_next_tile_id = PPU_read(VRAM_OFFSET | vram_addr);
                    break;
                case 2:
                    bg_next_at = PPU_read(
                            VRAMA_OFFSET | (vram_addr & 0x0C00) | ((vram_addr >> 4) & 0x38) |
                            ((vram_addr >> 2) & 0x7));

                    bg_next_at_tile =
                            (bg_next_at >> (2 * (((vram_addr >> 5) & 0x2) | ((vram_addr >> 1) & 0x1)))) & 0x3;

                    break;
                case 4:
                    bg_next_tile_lsb = PPU_read(
                            CHR_ROM_OFFSET[control.pattern_background] + (bg_next_tile_id << 4) + v_vram.fine_y +
                            0);
                    break;
                case 6:
                    bg_next_tile_msb = PPU_read(
                            CHR_ROM_OFFSET[control.pattern_background] + (bg_next_tile_id << 4) + v_vram.fine_y +
                            8);
                    break;
                case 7:
                    incrementalScrollX();
                    break;
                default:
                    break;
            }
        }
        if (cycle == 256) {
            incrementalScrollY();
        }
        if (cycle == 257) {
            transferAddressX();
        }
    }

    if ((0 <= scanline && scanline <= 239)) {
        if (cycle == 258) {
            spriteZeroHitPosible = 0;
            sprite_count = 0;
            for (int s = 0; s < 32; s++) {
                sOAM[s] = 0xFF;
            }

            int s = 0;
            for (int n = 0; n < 64; n++) {
                if (pOAM[4 * n] <= scanline && scanline <= (pOAM[4 * n] + 7)) {

                    if (sprite_count < 8) {
                        if (n == 0) {
                            spriteZeroHitPosible = 1;
                        }
                        for (int m = 0; m < 4; m++) {
                            sOAM[s++] = pOAM[4 * n + m];
                        }
                    }
                    sprite_count++;
                }
            }
            PPU_status.sprite_overflow = (sprite_count > 8);
            sprite_count = (sprite_count > 8) ? 8 : sprite_count;

        }
        if (cycle == 340) {
            for (uint8_t i = 0; i < sprite_count; i++) {
                uint8_t sp_pattern_lo, sp_pattern_hi;
                uint16_t sp_pattern_addr_lo, sp_pattern_addr_hi;
                if ((sOAM[i * 4 + 2] & 0x80)) {
                    sp_pattern_addr_lo = ((uint16_t)(control.pattern_sprite) << 12)
                                         | ((uint16_t)(sOAM[i * 4 + 1]) << 4)
                                         | (7 - ((scanline - sOAM[i * 4 + 0]) & 0x07));
                } else {
                    sp_pattern_addr_lo = ((uint16_t)(control.pattern_sprite) << 12)
                                         | ((uint16_t)(sOAM[i * 4 + 1]) << 4)
                                         | ((scanline - sOAM[i * 4 + 0]) & 0x07);
                }
                sp_pattern_addr_hi = sp_pattern_addr_lo + 8;
                sp_pattern_lo = PPU_read(sp_pattern_addr_lo);
                sp_pattern_hi = PPU_read(sp_pattern_addr_hi);

                if ((sOAM[i * 4 + 2] & 0x40)) {
                    sp_pattern_lo = bitswap(sp_pattern_lo);
                    sp_pattern_hi = bitswap(sp_pattern_hi);
                }
                sp_shifter_pattern_lo[i] = sp_pattern_lo;
                sp_shifter_pattern_hi[i] = sp_pattern_hi;
            }
        }
    }
    if (0 <= scanline && scanline <= 239) {
        if (1 <= cycle && cycle <= 256) {
            render();
            uint16_t c = ppuColor[color];
            *DATA = c;
            *DATA = c >> 8;
        }
    }

    if (cycle == 1 && scanline == 241) {
        PPU_status.vertical_blank = 1;
        frame++;

        if (control.enable_nmi)
            nmi = 1;

    }

    if (cycle == 1 && scanline == 261) {
        PPU_status.vertical_blank = 0;
        PPU_status.sprite_overflow = 0;
        PPU_status.sprite_zero_hit = 0;

        for (int i = 0; i < 8; i++) {
            sp_shifter_pattern_hi[i] = 0;
            sp_shifter_pattern_lo[i] = 0;
        }
    }

    if (scanline == 261) {
        if (280 <= cycle && cycle <= 304) {
            transferAddressY();
        }
    }


    cycle++;
    if (cycle > 340) {
        cycle = 0;
        scanline++;
        if (scanline > 261) {
            scanline = 0;
        }
    }

}

uint8_t PPU_read(uint16_t addr) {
    if (addr >= 0x0000 && addr <= 0x1FFF)
        return CHR_ROM[addr & 0x1FFF];
    else if (addr >= 0x2000 && addr <= 0x3EFF)
        return VRAM[addr & 0x0FFF];
    else if (addr >= 0x3F00 && addr <= 0x3F1F) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;

        if (addr == 0x0004 || addr == 0x0008 || addr == 0x000C) {
            addr = 0x0000;
        }
        return palette[addr];
    }
    return 0;
}

void PPU_write(uint16_t addr, uint8_t src) {
    if (addr >= 0x0000 && addr <= 0x1FFF)
        CHR_ROM[addr & 0x1FFF] = src;
    else if (addr >= 0x2000 && addr <= 0x3EFF)
        VRAM[addr & 0x0FFF] = src;
    else if (addr >= 0x3F00 && addr <= 0x3F1F) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        palette[addr] = src;
    }
}

uint8_t PPU_cpuRead(uint16_t addr) {
    uint8_t data = 0;
    switch (addr) {
        case 0x0000:
            data = control.reg;
            break;
        case 0x0001:
            data = mask.reg;
            break;
        case 0x0002:
            data = PPU_status.reg & 0xE0;
            PPU_status.vertical_blank = 0;
            address_latch = 0;
            break;
        case 0x0003:
            break;
        case 0x0004:
            data = pOAM[oam_addr];
            oam_addr++;
            break;
        case 0x0005:
            break;
        case 0x0006:
            break;
        case 0x0007:
            data = ppuDataBuffer;
            ppuDataBuffer = PPU_read(v_vram.reg & 0x3FFF);
            if (v_vram.reg >= 0x3f00) data = ppuDataBuffer;
            v_vram.reg += (control.increment_mode ? 32 : 1);
            break;

    }
    return data;
}

void PPU_cpuWrite(uint16_t addr, uint8_t src) {
    switch (addr) {
        case 0x0000:
            control.reg = src;
            t_vram.nametable_x = control.nametable_x;
            t_vram.nametable_y = control.nametable_y;
            break;
        case 0x0001:
            mask.reg = src;
            break;
        case 0x0002:
            PPU_status.reg = src;
            break;
        case 0x0003:
            oam_addr = src;
            break;
        case 0x0004:
            pOAM[oam_addr] = src;
            oam_addr++;
            break;
        case 0x0005:
            if (address_latch == 0) {
                t_vram.coarse_x = (src >> 3) & 0x1F;
                fine_x = src & 0x7;
                address_latch = 1;
            } else {
                t_vram.coarse_y = (src >> 3) & 0x1F;
                t_vram.fine_y = src & 0x7;
                address_latch = 0;
            }
            break;
        case 0x0006:
            if (address_latch == 0) {
                t_vram.reg = ((uint16_t)(src & 0x3F) << 8) | (t_vram.reg & 0x00FF);
                address_latch = 1;
            } else {
                t_vram.reg = (t_vram.reg & 0xFF00) | src;
                v_vram.reg = t_vram.reg;
                address_latch = 0;
            }

            break;
        case 0x0007:
            PPU_write(v_vram.reg & 0x3FFF, src);
            v_vram.reg += (control.increment_mode ? 32 : 1);
            break;
    }
}
