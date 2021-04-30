#include "utils.h"
#include "lcd.h"
#include "imu.h"
#include "cam.h"
#include "pcm.h"

extern void lcd_demo();
extern void imu_demo();
extern void cam_demo();
extern void pcm_demo();

int main(int argc, char **argv)
{   
    pcm_demo();

    return 0;
}
