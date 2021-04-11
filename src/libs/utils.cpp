#include "utils.h"

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



int clz(uint32_t x)
{
    static const char debruijn32[32] = {
        0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
        1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
    };
    x |= x>>1;
    x |= x>>2;
    x |= x>>4;
    x |= x>>8;
    x |= x>>16;
    x++;
    return debruijn32[x*0x076be629>>27];
}

int nsec2msec(long nsec, unsigned int divider)
{
    signed int sign = nsec ^ divider;
    if ( (divider & 0x80000000) != 0 )
        divider = -divider;
    if ( divider == 1 )
    {
    if ( (sign ^ nsec) < 0 )
        nsec = -nsec;
    }
    else
    {
        unsigned int t = nsec;
        if ( nsec < 0 )
            t = -nsec;
        if ( t <= divider )
        {
            if ( t < divider )
                nsec = 0;
            if ( t == divider )
                nsec = (sign >> 31) | 1;
        }
        else if ( (divider & (divider - 1)) != 0 )
        {
            char bit = clz(divider) - clz(t);
            unsigned int v5 = divider << bit;
            unsigned int v6 = 1 << bit;
            nsec = 0;
            while ( 1 )
            {
                if ( t >= v5 )
                {
                    t -= v5;
                    nsec |= v6;
                }
                if ( t >= v5 >> 1 )
                {
                    t -= v5 >> 1;
                    nsec |= v6 >> 1;
                }
                if ( t >= v5 >> 2 )
                {
                    t -= v5 >> 2;
                    nsec |= v6 >> 2;
                }
                if ( t >= v5 >> 3 )
                {
                    t -= v5 >> 3;
                    nsec |= v6 >> 3;
                }
                bool isZero = t == 0;
                if ( t )
                {
                    v6 >>= 4;
                    isZero = v6 == 0;
                }
                if ( isZero )
                    break;
                v5 >>= 4;
            }
            if ( sign < 0 )
                nsec = -nsec;
        }
        else
        {
            nsec = t >> (31 - clz(divider));
            if ( sign < 0 )
                nsec = -nsec;
        }
    }
    return nsec;
}

int64_t get_time()
{
    struct timespec tp;
    // clk_id = CLOCK_MONOTONIC; //1
    // clk_id = CLOCK_BOOTTIME; //7
    if ( clock_gettime(CLOCK_BOOTTIME, &tp) )
        return 0LL;
    long nsec = nsec2msec(tp.tv_nsec, 1000000u);
    return 1000LL * tp.tv_sec + nsec;
}

void delay(uint ms)
{
    usleep(ms * 1000);
}
