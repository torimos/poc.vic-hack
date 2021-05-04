#include "utils.h"

extern void lcd_demo();
extern void imu_demo();
extern void cam_demo();
extern void pcm_demo();
extern void spine_demo();

int main(int argc, char **argv) {
    spine_demo();
    return 0;
}
