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
// int (*mm_app_stop_preview) (mm_camera_test_obj_t *test_obj);
// int (*mm_app_start_capture_raw) (mm_camera_test_obj_t *test_obj, uint8_t num_snapshots);
// int (*mm_app_stop_capture_raw) (mm_camera_test_obj_t *test_obj);
int (*mm_app_start_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
int (*mm_app_stop_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
mm_camera_channel_t* (*mm_app_add_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_type_t ch_type, mm_camera_channel_attr_t *attr, mm_camera_buf_notify_t channel_cb, void *userdata);
int (*mm_app_del_channel) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
mm_camera_stream_t* (*mm_app_add_stream) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel);
int (*mm_app_del_stream) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel, mm_camera_stream_t *stream);
// int (*mm_app_stream_initbuf) (cam_frame_len_offset_t *frame_offset_info,
//                           uint8_t *num_bufs,
//                           uint8_t **initial_reg_flag,
//                           mm_camera_buf_def_t **bufs,
//                           mm_camera_map_unmap_ops_tbl_t *ops_tbl,
//                           void *user_data);
// int32_t (*mm_app_stream_deinitbuf) (mm_camera_map_unmap_ops_tbl_t *ops_tbl, void *user_data);
// int32_t (*mm_app_stream_clean_invalidate_buf) (uint32_t index, void *user_data);
// int32_t (*mm_app_stream_invalidate_buf) (uint32_t index, void *user_data);
int (*mm_app_config_stream)(mm_camera_test_obj_t *test_obj,
                         mm_camera_channel_t *channel,
                         mm_camera_stream_t *stream,
                         mm_camera_stream_config_t *config);
mm_camera_channel_t* (*mm_app_get_channel_by_type) (mm_camera_test_obj_t *test_obj,
                                                 mm_camera_channel_type_t ch_type);
// int (*mm_app_alloc_bufs) (mm_camera_app_buf_t* app_bufs,
//                       cam_frame_len_offset_t *frame_offset_info,
//                       uint8_t num_bufs,
//                       uint8_t is_streambuf,
//                       size_t multipleOf);
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
int (*mm_app_stop_and_del_channel) (mm_camera_test_obj_t *test_obj,
                                       mm_camera_channel_t *channel);

int lib_init(mm_camera_lib_handle* lib_handle)
{
    void* ptr = dlopen("libmm-qcamera.so", RTLD_NOW);
    *(void **)&(mm_camera_lib_open) = dlsym(ptr, "mm_camera_lib_open");
    // *(void **)&(mm_app_stop_preview) = dlsym(ptr, "mm_app_stop_preview");
    // *(void **)&(mm_app_start_capture_raw) = dlsym(ptr, "mm_app_start_capture_raw");
    // *(void **)&(mm_app_stop_capture_raw) = dlsym(ptr, "mm_app_stop_capture_raw");

    *(void **)&(mm_app_start_channel) = dlsym(ptr, "mm_app_start_channel");
    *(void **)&(mm_app_stop_channel) = dlsym(ptr, "mm_app_stop_channel");
    *(void **)&(mm_app_add_channel) = dlsym(ptr, "mm_app_add_channel");
    *(void **)&(mm_app_del_channel) = dlsym(ptr, "mm_app_del_channel");
    *(void **)&(mm_app_add_stream) = dlsym(ptr, "mm_app_add_stream");
    *(void **)&(mm_app_del_stream) = dlsym(ptr, "mm_app_del_stream");
    // *(void **)&(mm_app_stream_initbuf) = dlsym(ptr, "mm_app_stream_initbuf");
    // *(void **)&(mm_app_stream_deinitbuf) = dlsym(ptr, "mm_app_stream_deinitbuf");
    // *(void **)&(mm_app_stream_clean_invalidate_buf) = dlsym(ptr, "mm_app_stream_clean_invalidate_buf");
    // *(void **)&(mm_app_stream_invalidate_buf) = dlsym(ptr, "mm_app_stream_invalidate_buf");
    *(void **)&(mm_app_get_channel_by_type) = dlsym(ptr, "mm_app_get_channel_by_type");
    *(void **)&(mm_app_config_stream) = dlsym(ptr, "mm_app_config_stream");
    // *(void **)&(mm_app_alloc_bufs) = dlsym(ptr, "mm_app_alloc_bufs");
    *(void **)&(mm_app_add_snapshot_stream) = dlsym(ptr, "mm_app_add_snapshot_stream");
    *(void **)&(mm_app_add_raw_stream) = dlsym(ptr, "mm_app_add_raw_stream");
    *(void **)&(mm_app_stop_and_del_channel) = dlsym(ptr, "mm_app_stop_and_del_channel");
    printf("%s: libmm-qcamera.so lib initialized\n", __func__);

    int r = mm_camera_lib_open(lib_handle, 0);
    if ( r )
    {
        printf("%s:mm_camera_lib_open() err=%d\n", __func__, r);
        return -1;
    }
    // else if ( !lib_handle->app_ctx.num_cameras )
    // {
    //     printf("%s: No camera sensors reported!\n", __func__);
    //     return -2;
    // }
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

// void mm_app_dump_frame(mm_camera_buf_def_t *frame,
//                        const char *name,
//                        const char *ext,
//                        uint32_t frame_idx)
// {
//     char file_name[FILENAME_MAX];
//     int file_fd;
//     int i;
//     int offset = 0;
//     if ( frame != NULL) {
//         snprintf(file_name, sizeof(file_name), "/hack/%s_%04d.%s", name, frame_idx, ext);
//         file_fd = open(file_name, O_RDWR | O_CREAT, 0777);
//         if (file_fd < 0) {
//             printf("%s: ERROR. cannot open file %s \n", __func__, file_name);
//         } else {
//             for (i = 0; i < frame->planes_buf.num_planes; i++) {
//                 printf("%s: saving file from address: %p, data offset: %d, "
//                      "length: %d \n", __func__, frame->buffer,
//                     frame->planes_buf.planes[i].data_offset, frame->planes_buf.planes[i].length);
//                 write(file_fd,
//                       (uint8_t *)frame->buffer + offset,
//                       frame->planes_buf.planes[i].length);
//                 offset += (int)frame->planes_buf.planes[i].length;
//             }

//             close(file_fd);
//             printf("dump %s", file_name);
//         }
//     }
// }

static void mm_anki_app_rdi_notify_cb(void *bufs, void *user_data)//mm_camera_super_buf_t
{

    int rc;
    uint32_t i = 0;
    mm_camera_test_obj_t *pme = (mm_camera_test_obj_t *)user_data;

    printf("%s: BEGIN\n", __func__);

    mm_camera_app_done();

    printf("%s: END\n", __func__);
}


void validate(mm_camera_lib_handle* h)
{
    printf("%s: stream_width: %d\n", __func__, h->current_params.stream_width);
    printf("%s: stream_height: %d\n", __func__, h->current_params.stream_height);
    printf("%s: af_mode: %d\n", __func__, h->current_params.af_mode);
    printf("%s: buffer_width: %d\n", __func__, h->test_obj.buffer_width);
    printf("%s: buffer_height: %d\n", __func__, h->test_obj.buffer_height);
    printf("%s: buffer_format: %d\n", __func__, h->test_obj.buffer_format);
    
    printf("%s: cam->camera_handle: %x\n", __func__, h->test_obj.cam->camera_handle);
    printf("%s: cam->ops: %x\n", __func__, h->test_obj.cam->ops);

    printf("%s: cap_buf.mem_info.fd: %x\n", __func__, h->test_obj.cap_buf.mem_info.fd);
    printf("%s: cap_buf.mem_info.main_ion_fd: %x\n", __func__, h->test_obj.cap_buf.mem_info.main_ion_fd);
    printf("%s: cap_buf.mem_info.handle: %x\n", __func__, h->test_obj.cap_buf.mem_info.handle);
    printf("%s: cap_buf.mem_info.size: %x\n", __func__, h->test_obj.cap_buf.mem_info.size);
    printf("%s: cap_buf.mem_info.data: %x\n", __func__, h->test_obj.cap_buf.mem_info.data);

    printf("%s: parm_buf.mem_info.fd: %x\n", __func__, h->test_obj.parm_buf.mem_info.fd);
    printf("%s: parm_buf.mem_info.main_ion_fd: %x\n", __func__, h->test_obj.parm_buf.mem_info.main_ion_fd);
    printf("%s: parm_buf.mem_info.handle: %x\n", __func__, h->test_obj.parm_buf.mem_info.handle);
    printf("%s: parm_buf.mem_info.size: %x\n", __func__, h->test_obj.parm_buf.mem_info.size);
    printf("%s: parm_buf.mem_info.data: %x\n", __func__, h->test_obj.parm_buf.mem_info.data);
    printf("%s: params_buffer: %x\n", __func__, h->test_obj.params_buffer);
    
    printf("%s: test_obj.jpeg_hdl: %x\n", __func__, h->test_obj.jpeg_hdl);
        
    //mem_dump("xxx", &h->test_obj, 256);
}

mm_camera_stream_t * mm_my_app_add_rdi_stream(mm_camera_test_obj_t *test_obj,
                                               mm_camera_channel_t *channel,
                                               mm_camera_buf_notify_t stream_cb,
                                               void *userdata,
                                               uint8_t num_bufs,
                                               uint8_t num_burst)
{
    mm_camera_stream_t *stream = NULL;

    stream = mm_app_add_stream(test_obj, channel);
    if (NULL == stream) {
        printf("%s: ERROR. add stream failed\n", __func__);
        return NULL;
    }

    // int rc = mm_app_config_stream(test_obj, channel, stream, &stream->s_config);
    // if (MM_CAMERA_OK != rc) {
    //     printf("%s: ERROR. config rdi stream err=%d\n", __func__, rc);
    //     return NULL;
    // }
    return NULL;
}

mm_camera_channel_t * mm_my_app_add_rdi_channel(mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t notify_cb)
{
    mm_camera_channel_t *channel = 0;
    mm_camera_stream_t *stream = 0;

    channel = mm_app_add_channel(test_obj,
                                 MM_CHANNEL_TYPE_RDI,
                                 0,
                                 0,
                                 0);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return NULL;
    }

    stream = mm_my_app_add_rdi_stream(test_obj, 
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

    return channel;
}

int mm_my_app_start_rdi(mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t notify_cb)
{
    int rc = MM_CAMERA_OK;
    mm_camera_channel_t *channel = mm_my_app_add_rdi_channel(test_obj, num_burst, notify_cb);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return -MM_CAMERA_E_GENERAL;
    }

    rc = mm_app_start_channel(test_obj, channel);
    if (MM_CAMERA_OK != rc) {
        printf("%s: ERROR. start rdi failed rc=%d\n", __func__, rc);
        mm_app_del_channel(test_obj, channel);
        return rc;
    }

    return rc;
}


int main(int argc, char **argv)
{
    printf("%s: camera test\n", __func__);

    mm_camera_lib_handle handle;
    GPIO cenPin, sbyPin;
    gpioInit(&cenPin, 83, 1, 1); //poweron
    gpioInit(&sbyPin, 94, 1, 0); //unpause
    printf("%s: gpio initialized\n", __func__);

    
    printf("%s: mm_camera_lib_handle sz: %d == 480320\n", __func__, sizeof(mm_camera_lib_handle));
    printf("%s: mm_camera_test_obj_t sz: %d == 480256\n", __func__, sizeof(mm_camera_test_obj_t));
    printf("%s: mm_camera_stream_t sz: %d == 14020\n", __func__, sizeof(mm_camera_stream_t));
    printf("%s: mm_camera_channel_t sz: %d == 56088\n", __func__, sizeof(mm_camera_channel_t));
    printf("%s: mm_camera_stream_config_t sz: %d == ???\n", __func__, sizeof(mm_camera_stream_config_t));
    return -1;
    if (!lib_init(&handle))
    {
        validate(&handle);
        // int rc = mm_my_app_start_rdi(&handle.test_obj, 0, mm_anki_app_rdi_notify_cb);
        // if (!rc)
        // {
        //     printf("%s: mm_anki_app_start_rdi: ok\n", __func__);
        //     mm_camera_app_wait();
        //     printf("%s: mm_camera_app_wait: done\n", __func__);
        // }


        // //if (!mm_app_stop_preview(&handle.test_obj))
        // {
        // //    printf("%s: mm_app_stop_preview: ok\n", __func__);
        //     if (!mm_app_start_capture_raw(&handle.test_obj, 5))
        //     {
        //         printf("%s: mm_app_start_capture_raw: ok\n", __func__);
        //         mm_camera_app_wait();
        //         printf("%s: mm_camera_app_wait: ok\n", __func__);
        //         if (!mm_app_stop_capture_raw(&handle.test_obj))
        //         {
        //             printf("%s: mm_app_stop_capture_raw: ok\n", __func__);
        //         }
        //     }
        // }
    }
    return 0;
}

