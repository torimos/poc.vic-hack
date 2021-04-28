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

int (*mm_app_config_stream) (mm_camera_test_obj_t *test_obj,
                         mm_camera_channel_t *channel,
                         mm_camera_stream_t *stream,
                         mm_camera_stream_config_t *config);


int (*mm_app_start_rdi) (mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t notify_cb);

int (*mm_app_cache_ops) (mm_camera_app_meminfo_t *mem_info,int cmd);

int (*mm_app_stream_initbuf) (cam_frame_len_offset_t *frame_offset_info,
                          uint8_t *num_bufs,
                          uint8_t **initial_reg_flag,
                          mm_camera_buf_def_t **bufs,
                          mm_camera_map_unmap_ops_tbl_t *ops_tbl,
                          void *user_data);
int32_t (*mm_app_stream_deinitbuf) (mm_camera_map_unmap_ops_tbl_t *ops_tbl,
                                void *user_data);
int32_t (*mm_app_stream_clean_invalidate_buf) (uint32_t index, void *user_data);
int32_t (*mm_app_stream_invalidate_buf) (uint32_t index, void *user_data);

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
    *(void **)&(mm_app_start_rdi) = dlsym(ptr, "mm_app_start_rdi");
    *(void **)&(mm_app_config_stream) = dlsym(ptr, "mm_app_config_stream");
    *(void **)&(mm_app_stream_initbuf) = dlsym(ptr, "mm_app_stream_initbuf");
    *(void **)&(mm_app_stream_deinitbuf) = dlsym(ptr, "mm_app_stream_deinitbuf");
    *(void **)&(mm_app_stream_clean_invalidate_buf) = dlsym(ptr, "mm_app_stream_clean_invalidate_buf");
    *(void **)&(mm_app_stream_invalidate_buf) = dlsym(ptr, "mm_app_stream_invalidate_buf");
    

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
        for (i = 0; i < frame->planes_buf.num_planes; i++) {
            printf("%s: saving file from address: %p, data offset: %d, length: %d\n",  
                __func__, frame->buffer, frame->planes_buf.planes[i].data_offset, frame->planes_buf.planes[i].length);
            snprintf(file_name, sizeof(file_name), "/hack/%s_%04d_%02d.%s", name, frame_idx, i, ext);
            file_fd = open(file_name, O_RDWR | O_CREAT, 0777);
            if (file_fd < 0) {
                printf("%s: ERROR. cannot open file %s \n", __func__, file_name);
            } else {
                write(file_fd, (uint8_t *)frame->buffer + offset, frame->planes_buf.planes[i].length);
                offset += (int)frame->planes_buf.planes[i].length;
                close(file_fd);
                printf("dump %s", file_name);
            }
        }
    }
}

typedef int32_t (*mm_camera_ops_qbuf_t) (uint32_t camera_handle, uint32_t ch_id, mm_camera_buf_def_t *buf);

static void mm_my_app_notify_cb(mm_camera_super_buf_t *bufs, void *user_data)
{
    int rc;
    uint32_t i = 0;
    mm_camera_test_obj_t *pme = (mm_camera_test_obj_t *) user_data;
    mm_camera_buf_def_t *frame = bufs->bufs[0];
    printf("%s: BEGIN - length=%zu, frame idx = %d stream_id=%d num_bufs=%d\n", 
        __func__, frame->frame_len, frame->frame_idx, frame->stream_id, bufs->num_bufs);
    mm_app_dump_frame(frame, "cam_dump", "raw", frame->frame_idx);

    ((mm_camera_ops_qbuf_t)pme->cam->ops[25])(bufs->camera_handle, bufs->ch_id, frame);
    mm_app_cache_ops((mm_camera_app_meminfo_t *)frame->mem_info, ION_IOC_INV_CACHES);

    mm_camera_app_done();

    printf("%s: END\n", __func__);
}

mm_camera_channel_t * mm_my_app_add_preview_channel(mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t stream_cb)
{
    printf("%s: begin\n", __func__);
    mm_camera_channel_t *channel = NULL;
    mm_camera_stream_t *stream = NULL;

    channel = mm_app_add_channel(test_obj,
                                 MM_CHANNEL_TYPE_PREVIEW,
                                 NULL,
                                 NULL,
                                 NULL);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_channel: ok\n", __func__);

    stream = mm_app_add_stream(test_obj, channel);
    if (stream == NULL)
    {
        printf("%s: ERROR. add stream failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_stream: ok\n", __func__);


    stream->s_config.mem_vtbl.get_bufs = mm_app_stream_initbuf;
    stream->s_config.mem_vtbl.put_bufs = mm_app_stream_deinitbuf;
    stream->s_config.mem_vtbl.clean_invalidate_buf = mm_app_stream_clean_invalidate_buf;
    stream->s_config.mem_vtbl.invalidate_buf = mm_app_stream_invalidate_buf;
    stream->s_config.mem_vtbl.user_data = (void *)stream;
    stream->s_config.stream_cb = stream_cb;
    stream->s_config.userdata = (void*)test_obj;
    stream->num_of_bufs = 7;

    stream->s_config.stream_info = (cam_stream_info_t *)stream->s_info_buf.buf.buffer;
    memset(stream->s_config.stream_info, 0, sizeof(cam_stream_info_t));

    int* sinfo = (int*)stream->s_config.stream_info->data;

    sinfo[1] = CAM_STREAM_TYPE_PREVIEW;
    sinfo[2] = CAM_FORMAT_YUV_420_NV21;
    sinfo[288] = 0; // CAM_STREAMING_MODE_CONTINUOUS

    sinfo[3] = 1280;
    sinfo[4] = 720;
    stream->s_config.padding_info.width_padding = 32;
    stream->s_config.padding_info.height_padding = 32;
    stream->s_config.padding_info.plane_padding = 32;

    int rc = mm_app_config_stream(test_obj, channel, stream, &stream->s_config);
    if (MM_CAMERA_OK != rc) {
        printf("%s: ERROR. config stream failed\n", __func__);
        mm_app_del_channel(test_obj, channel);
        return NULL;
    }
    printf("%s: mm_app_config_stream: ok\n", __func__);
    return channel;
}


mm_camera_channel_t * mm_my_app_add_rdi_channel(mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t stream_cb)
{
    printf("%s: begin\n", __func__);
    mm_camera_channel_t *channel = NULL;
    mm_camera_stream_t *stream = NULL;

    channel = mm_app_add_channel(test_obj,
                                 MM_CHANNEL_TYPE_RDI,
                                 NULL,
                                 NULL,
                                 NULL);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_channel: ok\n", __func__);

    stream = mm_app_add_stream(test_obj, channel);
    if (stream == NULL)
    {
        printf("%s: ERROR. add stream failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_stream: ok\n", __func__);


    stream->s_config.mem_vtbl.get_bufs = mm_app_stream_initbuf;
    stream->s_config.mem_vtbl.put_bufs = mm_app_stream_deinitbuf;
    stream->s_config.mem_vtbl.clean_invalidate_buf = mm_app_stream_clean_invalidate_buf;
    stream->s_config.mem_vtbl.invalidate_buf = mm_app_stream_invalidate_buf;
    stream->s_config.mem_vtbl.user_data = (void *)stream;
    stream->s_config.stream_cb = stream_cb;
    stream->s_config.userdata = (void*)test_obj;
    stream->num_of_bufs = 8;

    stream->s_config.stream_info = (cam_stream_info_t *)stream->s_info_buf.buf.buffer;
    memset(stream->s_config.stream_info, 0, sizeof(cam_stream_info_t));

    int* sinfo = (int*)stream->s_config.stream_info->data;
    sinfo[1] = CAM_STREAM_TYPE_RAW;
    sinfo[2] = CAM_FORMAT_BAYER_MIPI_RAW_10BPP_BGGR;
    sinfo[288] = 0; // CAM_STREAMING_MODE_CONTINUOUS

    sinfo[3] = 1280;
    sinfo[4] = 720;
    stream->s_config.padding_info.width_padding = 32;
    stream->s_config.padding_info.height_padding = 32;
    stream->s_config.padding_info.plane_padding = 32;

    int rc = mm_app_config_stream(test_obj, channel, stream, &stream->s_config);
    if (MM_CAMERA_OK != rc) {
        printf("%s: ERROR. config stream failed\n", __func__);
        mm_app_del_channel(test_obj, channel);
        return NULL;
    }
    printf("%s: mm_app_config_stream: ok\n", __func__);
    return channel;
}

int mm_my_app_start(mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t notify_cb)
{
    printf("%s: begin\n", __func__);
    int rc = MM_CAMERA_OK;
    mm_camera_channel_t *channel = mm_my_app_add_preview_channel(test_obj, num_burst, notify_cb);
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

    if (test("test_obj.cap_buf.buf", base, &x.test_obj.cap_buf.buf, 392672)) return -1;

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
        int rc = mm_my_app_start(&handle.test_obj, 0, mm_my_app_notify_cb);
        if (!rc)
        {
            printf("%s: mm_anki_app_start_rdi: ok\n", __func__);
            mm_camera_app_wait();
            //delay(1000);
            printf("%s: mm_camera_app_wait: done\n", __func__);
        }
    }
    return 0;
}
