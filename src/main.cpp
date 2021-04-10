#include "utils.h"
#include "lcd.h"

#define BUF_SZ 184*96
uint16_t buff[BUF_SZ];

int main(int argc, char **argv)
{
    int spi = lcd_init();

    uint16_t colors[2][5] = {
        {0xF800, 0x7E0, 0x1F, 0xFFFF, 0},
        {0x1F, 0xF800, 0x7E0, 0xFFFF, 0}
    };

    int c=0;
    int i=0;
    for(c=0;c<5;c++)
    {
        for(i=0;i<BUF_SZ;i++)
            buff[i] = i<BUF_SZ/2 ? colors[0][c] : colors[1][c];
        lcd_write_data(spi, (char *)buff, BUF_SZ*2);
        delay(250);
    }
    return 0;
}