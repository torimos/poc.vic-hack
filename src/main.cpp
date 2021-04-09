#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <cstring>
#include <unistd.h>

struct GPIO
{
    int vd;
    int pin;
};

#define BUF_SZ 184*96
ushort buff[BUF_SZ];
GPIO dcPin;
GPIO rstPin;


void gpioClose(GPIO* gpio)
{
    if (gpio->vd)
        close(gpio->vd);
}

void gpioSet(GPIO* gpio, int state)
{
    if ( gpio->pin >= 0 && gpio->vd )
    {
        write(gpio->vd, (state == 1) ? "1":"0", 1u);
        printf("Set pin %d state to %d\n", gpio->pin, state);
    }
    else 
    {
        perror("Error setting gpio value");
        exit(5);
    }
}

void gpioInit(GPIO* gpio, int pin, int mode, int state)
{
    char tmp[64];
    gpio->pin = -1;
    memset(tmp, 0, 64);
    sprintf(tmp, "/sys/class/gpio/gpio%d/direction", pin);
    int dd = open(tmp, 1);
    if ( dd )
    {
        write(dd, (mode == 0) ? "in" : "out", (mode == 0) ? 2 : 3);
        close(dd);

        memset(tmp, 0, 64);
        sprintf(tmp, "/sys/class/gpio/gpio%d/value", pin);
        gpio->vd = open(tmp, mode != 0);
        if ( gpio->vd )
        {
            gpio->pin = pin;
            printf("Set pin %d mode to %d\n", gpio->pin, mode);
            if (mode == 1) {
                gpioSet(gpio, state);
            }
        }
        else
        {
            perror("failed to init gpio");
            exit(3);
        }
    }
    else
    {
        perror("failed to init gpio");
        exit(4);
    }
}

void delay(uint ms)
{
    usleep(ms * 1000);
}


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
#define TFT_CMD_PRC			0xF7
#define TFT_CMD_3GAMMA_EN	0xF2

#define ST_CMD_VCOMS       0xBB
#define ST_CMD_FRCTRL2      0xC6
#define ST_CMD_PWCTR1		0xD0

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_MH  0x04

#define TFT_CASET		0x2A
#define TFT_PASET		0x2B
#define TFT_RAMWR		0x2C
#define TFT_RAMRD		0x2E
#define TFT_CMD_PIXFMT	0x3A

#define TFT_CMD_DELAY	0x80

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

#define DISP_COLOR_BITS_24	0x66
#define DISP_COLOR_BITS_18	0x66
#define DISP_COLOR_BITS_16	0x55
#define TFT_RGB_BGR 0x00

static const unsigned char ST7789V_init[] = {
  2,                   					        // 16 commands in list
//  TFT_CMD_SWRESET, TFT_CMD_DELAY, 200,      	//  1: Software reset, no args, w/200 ms delay   
//   TFT_CMD_FRMCTR2, 5, 0x0c, 0x0c, 0x00, 0x33, 0x33,
//   TFT_ENTRYM, 1, 0x45,
//   ST_CMD_VCOMS, 1, 0x2B,

//   TFT_CMD_PWCTR1, 1, 0x2C,
//   TFT_CMD_PWCTR3, 2, 0x01, 0xff,
//   TFT_CMD_PWCTR4, 1, 0x11,
//   TFT_CMD_PWCTR5, 1, 0x20,

//   ST_CMD_FRCTRL2, 1, 0x0f,
//   ST_CMD_PWCTR1, 2, 0xA4, 0xA1,
//   TFT_CMD_GMCTRP1, 14, 0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19,
//   TFT_CMD_GMCTRN1, 14, 0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19,

//   TFT_MADCTL, 1, (MADCTL_MY | TFT_RGB_BGR),			// Memory Access Control (orientation)
//   TFT_CMD_PIXFMT, 1, DISP_COLOR_BITS_16,            // *** INTERFACE PIXEL FORMAT: 0x66 -> 18 bit; 0x55 -> 16 bit
  TFT_CMD_SLPOUT, TFT_CMD_DELAY, 120,				//  Sleep out,	//  120 ms delay
  TFT_DISPON, TFT_CMD_DELAY, 120,
};


int lcd_cmd(int spi, unsigned char cmd, unsigned char* txbuf, int txlen)
{
    gpioSet(&dcPin, 0);
    struct spi_ioc_transfer xfer[2];
    memset(xfer, 0, sizeof xfer);
    xfer[0].tx_buf = (unsigned long)&cmd;
    xfer[0].len = 1;
    xfer[1].tx_buf = (unsigned long)txbuf;
    xfer[1].len = txlen;
    printf("SPI_CMD: 0x%02x. len: %d\n", cmd, txlen);
    return ioctl(spi, SPI_IOC_MESSAGE(2), xfer);
}

int lcd_data(int spi, unsigned char* txbuf, int txlen)
{
    gpioSet(&dcPin, 1);
    struct spi_ioc_transfer xfer[1];
    memset(xfer, 0, sizeof xfer);
    xfer[0].tx_buf = (unsigned long)txbuf;
    xfer[0].len = txlen;
    printf("SPI_DAT len: %d\n", txlen);
    return ioctl(spi, SPI_IOC_MESSAGE(1), xfer);
    //write(spi, txbuf, txlen);
    //printf("SPI_DAT len: %d\n", txlen);
    //return 0;
}

int commandList(int spi, const unsigned char *addr) {
  unsigned char  numCommands, numArgs, cmd;
  ushort ms;
  int status = 0;
  numCommands = *addr++;				// Number of commands to follow
  while(numCommands--) {				// For each command...
    cmd = *addr++;						// save command
    numArgs  = *addr++;					// Number of args to follow
    ms       = numArgs & TFT_CMD_DELAY;	// If high bit set, delay follows args
    numArgs &= ~TFT_CMD_DELAY;			// Mask out delay bit

	status = lcd_cmd(spi, cmd, (unsigned char*)addr, numArgs);

	addr += numArgs;

    if(ms) {
      ms = *addr++;              // Read post-command delay time (ms)
      if(ms == 255) ms = 500;    // If 255, delay for 500 ms
	  delay(ms);
    }
  }
  return status;
}

void lcd_backlight(int level)
{
    char str[3];
    sprintf(str, "%02d\n", level%11);
    FILE* left = fopen("/sys/class/leds/face-backlight-left/brightness", "w");
    if (left){
        fwrite(str, 1u, 3u, left);
        fclose(left);
    }
    else
    {
        perror("Failed to open left backlight");
    }
    FILE* right = fopen("/sys/class/leds/face-backlight-right/brightness", "w");
    if (right){
        fwrite(str, 1u, 3u, right);
        fclose(right);
    }
    else
    {
        perror("Failed to open right backlight");
    }
}

int main(int argc, char **argv)
{
    lcd_backlight(10);

    int fd = open("/dev/spidev1.0", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    gpioInit(&dcPin, 110, 1, 1);
    gpioInit(&rstPin, 55, 1, 1);

    delay(50);
    gpioSet(&rstPin, 0);
    delay(50);
    gpioSet(&rstPin, 1);
    delay(120);
    int status = commandList(fd, ST7789V_init);
    printf("LCD Initialized: %d\n", status);
    
    int i=0;
    for(i=0;i<BUF_SZ;i++)
        buff[i] = 0x1F1F;

    lcd_data(fd, (unsigned char*)buff, BUF_SZ*2);
    
    return 0;
}