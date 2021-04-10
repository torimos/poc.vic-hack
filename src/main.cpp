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

struct GPIO
{
    int vd;
    int pin;
    int is_x;
};

#define BUF_SZ 184*96
uint16_t buff[BUF_SZ];
GPIO dcPin;
GPIO rstPin;
GPIO xPin;
uint8_t mode = 0;
uint8_t bits = 0;
uint32_t speed = 0;

void gpioClose(GPIO* gpio)
{
    if (gpio->vd)
        close(gpio->vd);
}

void gpioDirection(GPIO* gpio, int mode)
{
    char tmp[64];
    if (gpio->pin)
    {
        memset(tmp, 0, 64);
        sprintf(tmp, "/sys/class/gpio/gpio%d/direction", gpio->pin);
        int dd = open(tmp, O_RDWR);
        if ( dd )
        {
            write(dd, (mode == 0) ? "in" : "out", (mode == 0) ? 2 : 3);
            close(dd);
        }
        else
        {
            perror("failed to init gpio");
            exit(6);
        }
    }
}

void gpioSet(GPIO* gpio, int state)
{
    if ( gpio->pin >= 0 && gpio->vd )
    {
        if (gpio->is_x) {
            gpioDirection(gpio, (state == 0));
        }
        else {
            write(gpio->vd, (state == 1) ? "1":"0", 1u);
        }
        // printf("Set pin %d state to %d\n", gpio->pin, state);
    }
    else 
    {
        perror("Error setting gpio value");
        exit(5);
    }
}

void gpioInit(GPIO* gpio, int pin, int mode, int state, int is_x)
{
    char tmp[64];
    gpio->pin = -1;
    memset(tmp, 0, 64);
    sprintf(tmp, "/sys/class/gpio/gpio%d/direction", pin);
    int dd = open(tmp, O_RDWR);
    if ( dd )
    {
        write(dd, (mode == 0) ? "in" : "out", (mode == 0) ? 2 : 3);
        close(dd);

        memset(tmp, 0, 64);
        sprintf(tmp, "/sys/class/gpio/gpio%d/value", pin);
        gpio->vd = open(tmp, O_RDWR);
        if ( gpio->vd )
        {
            gpio->pin = pin;
            gpio->is_x = is_x;
            // printf("Set pin %d mode to %d\n", gpio->pin, mode);
            gpioSet(gpio, state);
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

int32_t lcd_read_reg(int fd, unsigned char reg)
{
    int32_t rez = 0;
    struct spi_ioc_transfer xfer[2];
    memset(xfer, 0, sizeof xfer);
    xfer[0].tx_buf = (unsigned long)&reg;
    xfer[0].len = 1;
    xfer[1].rx_buf = (unsigned long)&rez;
    xfer[1].len = sizeof(int32_t);
    ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
    return rez;
}

int lcd_data(int spi, int is_cmd, char* txbuf, int len)
{
    struct spi_ioc_transfer xfer[1];
    memset(xfer, 0, sizeof xfer);
    xfer[0].tx_buf = (unsigned long)txbuf;
    xfer[0].len = len;
    gpioSet(&dcPin, is_cmd == 0);
    // if (is_cmd)
    // {
    //     printf("LCD Command. Args Len:%d. ", len);
    //     int i;
    //     for(i=0;i<len;i++)
    //         printf("%02x ", txbuf[i]);
    //     printf("\n");
    // }
    // else {
    //     printf("LCD Data. Size:%d\n", len);
    // }
    return ioctl(spi, SPI_IOC_MESSAGE(1), xfer);
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

        status = lcd_data(spi, 1, (char*)&cmd, 1);
        if (numArgs > 0)
            status = lcd_data(spi, 0, (char *)addr, numArgs);

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
    int left = open("/sys/class/leds/face-backlight-left/brightness", O_RDWR);
    if (left){
        write(left, str, 3u);
        close(left);
    }
    else
    {
        perror("Failed to open left backlight");
    }
    int right = open("/sys/class/leds/face-backlight-right/brightness", O_RDWR);
    if (right){
        write(right, str, 3u);
        close(right);
    }
    else
    {
        perror("Failed to open right backlight");
    }
}

void lcd_hw_reset()
{
    gpioInit(&rstPin, 55, 0, 0, 1);
    gpioInit(&xPin, 96, 0, 0, 1);

    delay(50);
    gpioSet(&rstPin, 0);
    gpioSet(&xPin, 0);
    delay(50);
    gpioSet(&rstPin, 1);
    gpioSet(&xPin, 1);
    delay(120);

    gpioClose(&rstPin);
    gpioClose(&xPin);
}

int lcd_init()
{
    lcd_backlight(10);

    int spi = open("/dev/spidev1.0", O_RDWR);
    if (spi < 0) {
        perror("open");
        return 1;
    }

    ioctl(spi, SPI_IOC_RD_MODE, (char *)&mode);
    ioctl(spi, SPI_IOC_RD_BITS_PER_WORD, (char *)&bits);
    ioctl(spi, SPI_IOC_RD_MAX_SPEED_HZ, (char *)&speed);
    printf("LCD SPI Mode=%04x Bits=%02d Speed=%04d\n",mode,bits,speed);

    gpioInit(&dcPin, 110, 1, 1, 0);

    //lcd_hw_reset();
    
    int status = commandList(spi, ST7789V_init);
    return spi;
}

int main(int argc, char **argv)
{
    int spi = lcd_init();

    uint16_t colors1[3] = {0xF800, 0x7E0, 0x1F};
    uint16_t colors2[3] = {0x1F, 0xF800, 0x7E0};
    int c=0;
    int i=0;
    for(c=0;c<3;c++)
    {
        for(i=0;i<BUF_SZ;i++)
            buff[i] = i<BUF_SZ/2 ? colors1[c] : colors2[c];
        uint8_t cmd = TFT_RAMWR;
        lcd_data(spi, 1, (char*)&cmd, 1);
        int r = lcd_data(spi, 0, (char *)buff, BUF_SZ*2);
        delay(1000);
    }
    return 0;
}