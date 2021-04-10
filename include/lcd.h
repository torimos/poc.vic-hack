#ifndef __LCD_H
#define __LCD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define TFT_INVOFF     0x20
#define TFT_INVONN     0x21
#define TFT_DISPOFF    0x28
#define TFT_DISPON     0x29
#define TFT_MADCTL	   0x36
#define TFT_PTLAR 	   0x30
#define TFT_ENTRYM 	   0xB7

#define TFT_CMD_NOP			0x00
#define TFT_CMD_SWRESET		0x01
#define TFT_CMD_RDDID       0x04
#define TFT_CMD_RDDST		0x09

#define TFT_CMD_SLPIN		0x10
#define TFT_CMD_SLPOUT		0x11
#define TFT_CMD_PTLON		0x12
#define TFT_CMD_NORON		0x13

#define TFT_CMD_RDMODE		0x0A
#define TFT_CMD_RDMADCTL	0x0B
#define TFT_CMD_RDPIXFMT	0x0C
#define TFT_CMD_RDIMGFMT	0x0D
#define TFT_CMD_RDSELFDIAG  0x0F

#define TFT_CMD_GAMMASET	0x26

#define TFT_CMD_RAMCTRL		0xB0
#define TFT_CMD_FRMCTR1		0xB1
#define TFT_CMD_FRMCTR2		0xB2
#define TFT_CMD_FRMCTR3		0xB3
#define TFT_CMD_INVCTR		0xB4
#define TFT_CMD_DFUNCTR		0xB6

#define TFT_CMD_PWCTR1		0xC0
#define TFT_CMD_PWCTR2		0xC1
#define TFT_CMD_PWCTR3		0xC2
#define TFT_CMD_PWCTR4		0xC3
#define TFT_CMD_PWCTR5		0xC4
#define TFT_CMD_VMCTR1		0xC5
#define TFT_CMD_VMCTR2		0xC7

#define TFT_CMD_RDID1		0xDA
#define TFT_CMD_RDID2		0xDB
#define TFT_CMD_RDID3		0xDC
#define TFT_CMD_RDID4		0xDD

#define TFT_CMD_GMCTRP1		0xE0
#define TFT_CMD_GMCTRN1		0xE1

#define TFT_CMD_POWERA		0xCB
#define TFT_CMD_POWERB		0xCF
#define TFT_CMD_POWER_SEQ	0xED
#define TFT_CMD_DTCA		0xE8
#define TFT_CMD_DTCB		0xEA
#define TFT_CMD_EQCTRL      0xE9
#define TFT_CMD_PRC			0xF7
#define TFT_CMD_3GAMMA_EN	0xF2

#define ST_CMD_VCOMS       0xBB
#define ST_CMD_FRCTRL2      0xC6
#define ST_CMD_PWCTR1		0xD0

#define TFT_CASET		0x2A
#define TFT_RASET		0x2B
#define TFT_RAMWR		0x2C
#define TFT_RAMRD		0x2E
#define TFT_CMD_PIXFMT	0x3A

// Flags for TFT_MADCTL
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_RGB 0x00
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_SS  0x02
#define TFT_MAD_GS  0x01

#define DISP_COLOR_BITS_18	0x66
#define DISP_COLOR_BITS_16	0x55
#define TFT_RGB_BGR 0x00

#define TFT_CMD_DELAY	0x80

static const unsigned char ST7789V_init[] = {
    22,
    TFT_CMD_SWRESET, TFT_CMD_DELAY, 200,
    TFT_CASET, 4, 0, 0x1C, 0, 0xD3,
    TFT_RASET, 4, 0, 0, 0, 0x5F,
    TFT_MADCTL, 1, 0,
    TFT_CMD_PIXFMT, 1, DISP_COLOR_BITS_16,
    TFT_CMD_RAMCTRL, 2, 0, 8,
    TFT_CMD_FRMCTR2, 5, 0xC, 0xC, 0, 0x33, 0x33,
    TFT_ENTRYM, 1, 0x72,
    ST_CMD_VCOMS, 1, 0x3B,
    TFT_CMD_PWCTR1, 1, 0x2C,
    TFT_CMD_PWCTR3, 1, 0x1,
    TFT_CMD_PWCTR4, 1, 0x14,
    TFT_CMD_PWCTR5, 1, 0x20,
    ST_CMD_FRCTRL2, 1, 0xF,
    ST_CMD_PWCTR1, 2, 0xA4, 0xA1,
    TFT_CMD_GMCTRP1, 14, 0xD0,0x10,0x16,0xA,0xA,0x26,0x3C,0x53,0x53,0x18,0x15,0x12,0x36,0x3C,
    TFT_CMD_GMCTRN1, 14, 0xD0,0x11,0x19,0xA,0x9,0x25,0x3D,0x35,0x54,0x17,0x15,0x12,0x36,0x3C,
    TFT_CMD_EQCTRL, 3, 0x5, 0x5, 0x1,
    TFT_CMD_SLPOUT, TFT_CMD_DELAY, 120,
    TFT_INVONN, TFT_CMD_DELAY, 10,
    TFT_CMD_NORON, TFT_CMD_DELAY, 10,
    TFT_DISPON, TFT_CMD_DELAY, 250
};


int lcd_init();

int lcd_write_commands(int spi, const unsigned char *addr);
int lcd_write_data(int spi, char* data, int size);

#ifdef __cplusplus
}
#endif


#endif