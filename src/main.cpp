#include "utils.h"
#include "lcd.h"
#include "imu.h"
#include "mm_app.h"
#include <dlfcn.h>

extern void lcd_demo();
extern void imu_demo();


static pthread_mutex_t app_mutex;
static int thread_status = 0;
static pthread_cond_t app_cond_v;


int (*mm_camera_lib_open) (mm_camera_lib_handle *handle, int cam_id);
int (*mm_app_start_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
int (*mm_app_stop_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
mm_camera_channel_t* (*mm_app_add_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_type_t ch_type, mm_camera_channel_attr_t *attr, mm_camera_buf_notify_t channel_cb, void *userdata);
int (*mm_app_del_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
mm_camera_stream_t* (*mm_app_add_stream) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
int (*mm_app_del_stream) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel, mm_camera_stream_t *stream);
mm_camera_stream_t * (*mm_app_add_snapshot_stream) (
                                                mm_camera_test_obj_t *test_obj,
                                                mm_camera_channel_t *channel,
                                                mm_camera_buf_notify_t stream_cb,
                                                void *userdata,
                                                uint8_t num_bufs,
                                                uint8_t num_burst);
mm_camera_stream_t * (*mm_app_add_raw_stream) (mm_camera_test_obj_t *test_obj,
                                                mm_camera_channel_t *channel,
                                                mm_camera_buf_notify_t stream_cb,
                                                void *userdata,
                                                uint8_t num_bufs,
                                                uint8_t num_burst);

mm_camera_stream_t * (*mm_app_add_rdi_stream) (mm_camera_test_obj_t *test_obj,
                                               mm_camera_channel_t *channel,
                                               mm_camera_buf_notify_t stream_cb,
                                               void *userdata,
                                               uint8_t num_bufs,
                                               uint8_t num_burst);

int (*mm_app_cache_ops) (mm_camera_app_meminfo_t *mem_info,int cmd);

int lib_init(mm_camera_lib_handle* lib_handle)
{
    void* ptr = dlopen("libmm-qcamera.so", RTLD_NOW);
    *(void **)&(mm_camera_lib_open) = dlsym(ptr, "mm_camera_lib_open");
    *(void **)&(mm_app_start_channel) = dlsym(ptr, "mm_app_start_channel");
    *(void **)&(mm_app_stop_channel) = dlsym(ptr, "mm_app_stop_channel");
    *(void **)&(mm_app_add_channel) = dlsym(ptr, "mm_app_add_channel");
    *(void **)&(mm_app_del_channel) = dlsym(ptr, "mm_app_del_channel");
    *(void **)&(mm_app_add_stream) = dlsym(ptr, "mm_app_add_stream");
    *(void **)&(mm_app_del_stream) = dlsym(ptr, "mm_app_del_stream");
    *(void **)&(mm_app_add_snapshot_stream) = dlsym(ptr, "mm_app_add_snapshot_stream");
    *(void **)&(mm_app_add_raw_stream) = dlsym(ptr, "mm_app_add_raw_stream");
    *(void **)&(mm_app_add_rdi_stream) = dlsym(ptr, "mm_app_add_rdi_stream");
    *(void **)&(mm_app_cache_ops) = dlsym(ptr, "mm_app_cache_ops");
    

    printf("%s: libmm-qcamera.so lib initialized\n", __func__);

    int r = mm_camera_lib_open(lib_handle, 0);
    if ( r )
    {
        printf("%s:mm_camera_lib_open() err=%d\n", __func__, r);
        return -1;
    }
    else if ( !lib_handle->app_ctx.num_cameras )
    {
        printf("%s: No camera sensors reported!\n", __func__);
        return -2;
    }
    printf("%s: mm_camera_lib_open done!\n", __func__);
    return 0;
}

void mem_dump(const char* title, void *p, int size)
{
    printf("%s: %s\n", __func__, title);
    int i=0;
    for(i=0;i<size;i++)
        printf("%02X ", ((uint8_t*)p)[i]);
    printf("\n");
}

int mm_camera_app_wait()
{
    int rc = 0;
    pthread_mutex_lock(&app_mutex);
    if(0 == thread_status){
        pthread_cond_wait(&app_cond_v, &app_mutex);
    }
    thread_status = 0;
    pthread_mutex_unlock(&app_mutex);
    return rc;
}

void mm_camera_app_done()
{
  pthread_mutex_lock(&app_mutex);
  thread_status = 1;
  pthread_cond_signal(&app_cond_v);
  pthread_mutex_unlock(&app_mutex);
}

void mm_app_dump_frame(mm_camera_buf_def_t *frame,
                       const char *name,
                       const char *ext,
                       uint32_t frame_idx)
{
    char file_name[FILENAME_MAX];
    int file_fd;
    int i;
    int offset = 0;
    if ( frame != NULL) {
        snprintf(file_name, sizeof(file_name), "/hack/%s_%04d.%s", name, frame_idx, ext);
        file_fd = open(file_name, O_RDWR | O_CREAT, 0777);
        if (file_fd < 0) {
            printf("%s: ERROR. cannot open file %s \n", __func__, file_name);
        } else {
            for (i = 0; i < frame->planes_buf.num_planes; i++) {
                printf("%s: saving file from address: %p, data offset: %d, "
                     "length: %d \n", __func__, frame->buffer,
                    frame->planes_buf.planes[i].data_offset, frame->planes_buf.planes[i].length);
                write(file_fd,
                      (uint8_t *)frame->buffer + offset,
                      frame->planes_buf.planes[i].length);
                offset += (int)frame->planes_buf.planes[i].length;
            }

            close(file_fd);
            printf("dump %s", file_name);
        }
    }
}

static void mm_anki_app_rdi_notify_cb(mm_camera_super_buf_t *bufs, void *user_data)
{
    int rc;
    uint32_t i = 0;
    mm_camera_test_obj_t *pme = (mm_camera_test_obj_t *)user_data;
    mm_camera_buf_def_t *frame = bufs->bufs[0];
    printf("%s: BEGIN - length=%zu, frame idx = %d stream_id=%d\n", __func__, frame->frame_len, frame->frame_idx, frame->stream_id);
    mm_app_dump_frame(frame, "RDI_dump","raw", frame->frame_idx);
    mm_app_cache_ops((mm_camera_app_meminfo_t *)frame->mem_info, ION_IOC_INV_CACHES);

    mm_camera_app_done();

    printf("%s: END\n", __func__);
}

mm_camera_channel_t * mm_my_app_add_rdi_channel(mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t notify_cb)
{
    printf("%s: begin\n", __func__);
    mm_camera_channel_t *channel = NULL;
    mm_camera_stream_t *stream = NULL;

    channel = mm_app_add_channel(test_obj,
                                 MM_CHANNEL_TYPE_RDI,
                                 0,
                                 0,
                                 0);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_channel: ok\n", __func__);

    stream = mm_app_add_rdi_stream(test_obj, 
                                        channel, 
                                        notify_cb, 
                                        test_obj, 
                                        8, 
                                        num_burst);
    if (NULL == stream) {
        printf("%s: ERROR. add stream failed\n", __func__);
        mm_app_del_channel(test_obj, channel);
        return NULL;
    }
    printf("%s: mm_app_add_rdi_stream: ok\n", __func__);

    return channel;
}

int mm_my_app_start_rdi(mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t notify_cb)
{
    printf("%s: begin\n", __func__);
    int rc = MM_CAMERA_OK;
    mm_camera_channel_t *channel = mm_my_app_add_rdi_channel(test_obj, num_burst, notify_cb);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return -MM_CAMERA_E_GENERAL;
    }
    printf("%s: mm_my_app_add_rdi_channel: ok\n", __func__);

    rc = mm_app_start_channel(test_obj, channel);
    if (MM_CAMERA_OK != rc) {
        printf("%s: ERROR. start rdi failed rc=%d\n", __func__, rc);
        mm_app_del_channel(test_obj, channel);
        return rc;
    }
    printf("%s: mm_app_start_channel: ok\n", __func__);

    return rc;
}


int test(const char* t, int base, void* offset, int expected)
{
    int v = (int)offset - base;
    if (v != expected)
    {
        printf("%s: expected %d but got %d\n", t, expected, v);
        return 1;
    }
    return 0;
}

int validate()
{
    static mm_camera_lib_handle x;
    long base = (long)&x;
    if (test("test_obj.channels[0].streams[0].s_info_buf.buf.stream_type", base, &x.test_obj.channels[0].streams[0].s_info_buf.buf.stream_type, 13516)) return -1;
    if (test("test_obj.channels[0].streams[0].s_info_buf.buf.buf_type", base, &x.test_obj.channels[0].streams[0].s_info_buf.buf.buf_type, 13517)) return -1;
    if (test("test_obj.channels[0].streams[0].s_info_buf.buf.buf_idx", base, &x.test_obj.channels[0].streams[0].s_info_buf.buf.buf_idx, 13520)) return -1;
    if (test("test_obj.tune_data", base, &x.test_obj.tune_data, 395649)) return -1;
    if (test("tsctrl", base, &x.tsctrl, 480296)) return -1;
    if (sizeof(mm_camera_lib_handle) != 480320)
    {
        printf("%s: mm_camera_lib_handlesize expected %d but got %d\n", __func__, 480320, sizeof(mm_camera_lib_handle));
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    printf("%s: camera test\n", __func__);
    if (!validate()) printf("%s: validation - OK\n", __func__);
    else return -1;

    mm_camera_lib_handle handle;
    GPIO cenPin, sbyPin;
    gpioInit(&cenPin, 83, 1, 1); //poweron
    gpioInit(&sbyPin, 94, 1, 0); //unpause
    printf("%s: gpio initialized\n", __func__);

    if (!lib_init(&handle))
    {
        int rc = mm_my_app_start_rdi(&handle.test_obj, 0, mm_anki_app_rdi_notify_cb);
        if (!rc)
        {
            printf("%s: mm_anki_app_start_rdi: ok\n", __func__);
            mm_camera_app_wait();
            printf("%s: mm_camera_app_wait: done\n", __func__);
        }
    }
    return 0;
}
