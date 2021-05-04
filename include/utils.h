#ifndef __UTILS_H
#define __UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct GPIO
{
    int vd;
    int pin;
    bool open_drain;
};

void delay(uint ms);
void delay_us(ulong us);
int64_t get_time();

void gpioInit(GPIO* gpio, int pin, int mode, int state, bool is_x = false);
void gpioDirection(GPIO* gpio, int mode);
void gpioSet(GPIO* gpio, int state);
void gpioClose(GPIO* gpio);

uint32_t get_CRC32(uint8_t *data, int16_t len);

void debug_buffer(uint8_t* buffer, int buffer_size);

#ifdef __cplusplus
}
#endif

#endif //__UTILS_H