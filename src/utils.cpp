#include "utils.h"

void delay(uint ms)
{
    usleep(ms * 1000);
}

void gpioInit(GPIO* gpio, int pin, int mode, int state, bool is_x)
{
    char tmp[64];
    gpio->pin = -1;
    memset(tmp, 0, 64);
    sprintf(tmp, "/sys/class/gpio/gpio%d/direction", pin);
    int dd = open(tmp, O_RDWR);
    if ( dd )
    {
        (void)!write(dd, (mode == 0) ? "in" : "out", (mode == 0) ? 2 : 3);
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
            (void)!write(dd, (mode == 0) ? "in" : "out", (mode == 0) ? 2 : 3);
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
            (void)!write(gpio->vd, (state == 1) ? "1":"0", 1u);
        }
        // printf("Set pin %d state to %d\n", gpio->pin, state);
    }
    else 
    {
        perror("Error setting gpio value");
        exit(5);
    }
}

void gpioClose(GPIO* gpio)
{
    if (gpio->vd)
        close(gpio->vd);
}