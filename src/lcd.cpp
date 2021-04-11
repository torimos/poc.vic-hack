#include "lcd.h"
#include "utils.h"

GPIO dcPin;
GPIO rstPin;
GPIO xPin;
uint8_t mode = 0;
uint8_t bits = 8;
uint32_t speed = 19200000;

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

int lcd_cmd(int spi, uint8_t cmd)
{
    gpioSet(&dcPin, 0);
    return write(spi, &cmd, 1);
}

int lcd_data(int spi, char* data, int size)
{
   
    gpioSet(&dcPin, 1);
    return write(spi, data, size);
}

int lcd_write_commands(int spi, const unsigned char *addr) 
{
    unsigned char  numCommands, numArgs, cmd;
    ushort ms;
    int status = 0;
    numCommands = *addr++;				// Number of commands to follow
    while(numCommands--) {				// For each command...
        cmd = *addr++;						// save command
        numArgs  = *addr++;					// Number of args to follow
        ms       = numArgs & TFT_CMD_DELAY;	// If high bit set, delay follows args
        numArgs &= ~TFT_CMD_DELAY;			// Mask out delay bit

        status = lcd_cmd(spi, cmd);
        if (numArgs > 0)
            status = lcd_data(spi, (char *)addr, numArgs);

        addr += numArgs;
        if(ms) {
            ms = *addr++;              // Read post-command delay time (ms)
            if(ms == 255) ms = 500;    // If 255, delay for 500 ms
            delay(ms);
        }
    }
  return status;
}

int lcd_write_data(int spi, char* data, int size)
{
    lcd_cmd(spi, TFT_RAMWR);
    return lcd_data(spi, data, size);
}

void lcd_backlight(int level)
{
    char str[5];
    snprintf(str, sizeof(str), "%02d\n", (uint8_t)(level%255));
    int left = open("/sys/class/leds/face-backlight-left/brightness", O_RDWR);
    if (left){
        (void)!write(left, str, 3u);
        close(left);
    }
    else
    {
        perror("Failed to open left backlight");
    }
    int right = open("/sys/class/leds/face-backlight-right/brightness", O_RDWR);
    if (right){
        (void)!write(right, str, 3u);
        close(right);
    }
    else
    {
        perror("Failed to open right backlight");
    }
}

void lcd_hw_reset()
{
    gpioInit(&rstPin, 55, 0, 0, true);
    gpioInit(&xPin, 96, 0, 0, true);

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
    lcd_backlight(5);

    int spi = open("/dev/spidev1.0", O_RDWR);
    if (spi < 0) {
        perror("open");
        return 1;
    }
    ioctl(spi, SPI_IOC_WR_MODE, (char *)&mode);
    ioctl(spi, SPI_IOC_WR_BITS_PER_WORD, (char *)&bits);
    ioctl(spi, SPI_IOC_WR_MAX_SPEED_HZ, (char *)&speed);
    printf("LCD SPI Mode=%04x Bits=%02d Speed=%04d\n",mode,bits,speed);

    gpioInit(&dcPin, 110, 1, 1);

    lcd_hw_reset();
    
    lcd_write_commands(spi, ST7789V_init);
    return spi;
}