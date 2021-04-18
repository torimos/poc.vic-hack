#include "utils.h"
#include "lcd.h"
#include "imu.h"
#include <dlfcn.h>
#include "mm_camera_interface.h"

extern void lcd_demo();
extern void imu_demo();

typedef struct {
        void *ptr;
        uint8_t (*get_num_of_cameras) ();
        mm_camera_vtbl_t * (*camera_open) (uint8_t camera_idx);
} hal_lib_t;

int main(int argc, char **argv)
{
        printf("mm camera test\n");
        hal_lib_t hal_lib;
        hal_lib.ptr = dlopen("libmmcamera_interface.so", RTLD_NOW);
        *(void **)&(hal_lib.get_num_of_cameras) = dlsym(hal_lib.ptr, "get_num_of_cameras");
        *(void **)&(hal_lib.camera_open) = dlsym(hal_lib.ptr, "camera_open");
        printf("num_of_cameras: %d \n", hal_lib.get_num_of_cameras());


        mm_camera_vtbl_t *cameraHandle = NULL;
        cameraHandle = hal_lib.camera_open(0);
        if (!cameraHandle)
        {
                printf("camera_open failed. cam handle = %X\n", cameraHandle);
        }
        else {
                printf("camera_open. cam handle = %X\n", cameraHandle->camera_handle);
                printf("camera_open cam ops = %X\n", cameraHandle->ops);
                parm_buffer_t params;
                memset(&params, 0, sizeof(params));
                int rc = cameraHandle->ops->do_auto_focus(cameraHandle->camera_handle);
                if (rc != 0)
                {
                        printf("%s:autofocus error: %d\n", __func__, rc);
                }
        }
        
        return 0;
}