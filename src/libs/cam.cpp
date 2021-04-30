#include "cam.h"
#include "utils.h"
#include <dlfcn.h>
#include "mm_app.h"

static pthread_mutex_t app_mutex;
static int thread_status = 0;
static pthread_cond_t app_cond_v;

int lib_init(mm_cam_lib_t* lib)
{
    memset(lib, 0, sizeof(mm_cam_lib_t));
    void* ptr = dlopen("libmm-qcamera.so", RTLD_NOW);
    *(void **)&(lib->mm_camera_lib_open) = dlsym(ptr, "mm_camera_lib_open");
    *(void **)&(lib->mm_app_start_channel) = dlsym(ptr, "mm_app_start_channel");
    *(void **)&(lib->mm_app_stop_channel) = dlsym(ptr, "mm_app_stop_channel");
    *(void **)&(lib->mm_app_add_channel) = dlsym(ptr, "mm_app_add_channel");
    *(void **)&(lib->mm_app_del_channel) = dlsym(ptr, "mm_app_del_channel");
    *(void **)&(lib->mm_app_add_stream) = dlsym(ptr, "mm_app_add_stream");
    *(void **)&(lib->mm_app_del_stream) = dlsym(ptr, "mm_app_del_stream");
    *(void **)&(lib->mm_app_cache_ops) = dlsym(ptr, "mm_app_cache_ops");
    *(void **)&(lib->mm_app_start_rdi) = dlsym(ptr, "mm_app_start_rdi");

    *(void **)&(lib->mm_app_config_stream) = dlsym(ptr, "mm_app_config_stream");
    *(void **)&(lib->mm_app_stream_initbuf) = dlsym(ptr, "mm_app_stream_initbuf");
    *(void **)&(lib->mm_app_stream_deinitbuf) = dlsym(ptr, "mm_app_stream_deinitbuf");
    *(void **)&(lib->mm_app_stream_clean_invalidate_buf) = dlsym(ptr, "mm_app_stream_clean_invalidate_buf");
    *(void **)&(lib->mm_app_stream_invalidate_buf) = dlsym(ptr, "mm_app_stream_invalidate_buf");
    
    printf("%s: libmm-qcamera.so lib initialized\n", __func__);

    int r = lib->mm_camera_lib_open(&lib->handle, 0);
    if ( r )
    {
        printf("%s:mm_camera_lib_open() err=%d\n", __func__, r);
        return -1;
    }
    else if ( !lib->handle.app_ctx.num_cameras )
    {
        printf("%s: No camera sensors reported!\n", __func__);
        return -2;
    }
    printf("%s: mm_camera_lib_open done!\n", __func__);
    return 0;
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

mm_camera_channel_t * mm_camera_app_add_preview_channel(mm_cam_lib_t *lib, uint8_t num_burst, mm_camera_buf_notify_t stream_cb)
{
    printf("%s: begin\n", __func__);
    mm_camera_channel_t *channel = NULL;
    mm_camera_stream_t *stream = NULL;

    mm_camera_test_obj_t *test_obj = &lib->handle.test_obj;

    channel = lib->mm_app_add_channel(test_obj,
                                 MM_CHANNEL_TYPE_PREVIEW,
                                 NULL,
                                 NULL,
                                 NULL);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_channel: ok\n", __func__);

    stream = lib->mm_app_add_stream(test_obj, channel);
    if (stream == NULL)
    {
        printf("%s: ERROR. add stream failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_stream: ok\n", __func__);


    stream->s_config.mem_vtbl.get_bufs = lib->mm_app_stream_initbuf;
    stream->s_config.mem_vtbl.put_bufs = lib->mm_app_stream_deinitbuf;
    stream->s_config.mem_vtbl.clean_invalidate_buf = lib->mm_app_stream_clean_invalidate_buf;
    stream->s_config.mem_vtbl.invalidate_buf = lib->mm_app_stream_invalidate_buf;
    stream->s_config.mem_vtbl.user_data = (void *)stream;
    stream->s_config.stream_cb = stream_cb;
    stream->s_config.userdata = (void*)lib;
    stream->num_of_bufs = 7;

    stream->s_config.stream_info = (cam_stream_info_t *)stream->s_info_buf.buf.buffer;
    memset(stream->s_config.stream_info, 0, sizeof(cam_stream_info_t));

    stream->s_config.stream_info->stream_type = CAM_STREAM_TYPE_PREVIEW;
    stream->s_config.stream_info->fmt = CAM_FORMAT_YUV_420_NV21;

    stream->s_config.stream_info->dim.width = 1280;
    stream->s_config.stream_info->dim.height = 720;
    stream->s_config.padding_info.width_padding =
        stream->s_config.padding_info.height_padding =
            stream->s_config.padding_info.plane_padding = 32;

    if (num_burst == 0) {
        stream->s_config.stream_info->streaming_mode = CAM_STREAMING_MODE_CONTINUOUS;
    }
    else {
        stream->s_config.stream_info->streaming_mode = CAM_STREAMING_MODE_BURST;
        stream->s_config.stream_info->num_of_burst = num_burst;
    }

    int rc = lib->mm_app_config_stream(test_obj, channel, stream, &stream->s_config);
    if (MM_CAMERA_OK != rc) {
        printf("%s: ERROR. config stream failed\n", __func__);
        lib->mm_app_del_channel(test_obj, channel);
        return NULL;
    }
    printf("%s: mm_app_config_stream: ok\n", __func__);
    return channel;
}

mm_camera_channel_t * mm_camera_app_add_rdi_channel(mm_cam_lib_t *lib, uint8_t num_burst, mm_camera_buf_notify_t stream_cb)
{
    printf("%s: begin\n", __func__);
    mm_camera_channel_t *channel = NULL;
    mm_camera_stream_t *stream = NULL;

    mm_camera_test_obj_t *test_obj = &lib->handle.test_obj;

    channel = lib->mm_app_add_channel(test_obj,
                                 MM_CHANNEL_TYPE_RDI,
                                 NULL,
                                 NULL,
                                 NULL);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_channel: ok\n", __func__);

    stream = lib->mm_app_add_stream(test_obj, channel);
    if (stream == NULL)
    {
        printf("%s: ERROR. add stream failed\n", __func__);
        return NULL;
    }
    printf("%s: mm_app_add_stream: ok\n", __func__);


    stream->s_config.mem_vtbl.get_bufs = lib->mm_app_stream_initbuf;
    stream->s_config.mem_vtbl.put_bufs = lib->mm_app_stream_deinitbuf;
    stream->s_config.mem_vtbl.clean_invalidate_buf = lib->mm_app_stream_clean_invalidate_buf;
    stream->s_config.mem_vtbl.invalidate_buf = lib->mm_app_stream_invalidate_buf;
    stream->s_config.mem_vtbl.user_data = (void *)stream;
    stream->s_config.stream_cb = stream_cb;
    stream->s_config.userdata = (void*)lib;
    stream->num_of_bufs = 8;

    stream->s_config.stream_info = (cam_stream_info_t *)stream->s_info_buf.buf.buffer;
    memset(stream->s_config.stream_info, 0, sizeof(cam_stream_info_t));

    stream->s_config.stream_info->fmt = CAM_FORMAT_BAYER_MIPI_RAW_10BPP_BGGR;
    stream->s_config.stream_info->stream_type = CAM_STREAM_TYPE_RAW;

    stream->s_config.stream_info->dim.width = 1280;
    stream->s_config.stream_info->dim.height = 720;
    stream->s_config.padding_info.width_padding =
        stream->s_config.padding_info.height_padding =
            stream->s_config.padding_info.plane_padding = 0;
    
    if (num_burst == 0) {
        stream->s_config.stream_info->streaming_mode = CAM_STREAMING_MODE_CONTINUOUS;
    }
    else {
        stream->s_config.stream_info->streaming_mode = CAM_STREAMING_MODE_BURST;
        stream->s_config.stream_info->num_of_burst = num_burst;
    }

    int rc = lib->mm_app_config_stream(test_obj, channel, stream, &stream->s_config);
    if (MM_CAMERA_OK != rc) {
        printf("%s: ERROR. config stream failed\n", __func__);
        lib->mm_app_del_channel(test_obj, channel);
        return NULL;
    }
    printf("%s: mm_app_config_stream: ok\n", __func__);
    return channel;
}

int mm_camera_app_start(mm_cam_lib_t *lib, uint8_t num_burst, mm_camera_buf_notify_t notify_cb)
{
    printf("%s: begin\n", __func__);
    int rc = MM_CAMERA_OK;
    mm_camera_test_obj_t *test_obj = &lib->handle.test_obj;
    mm_camera_channel_t *channel = mm_camera_app_add_preview_channel(lib, num_burst, notify_cb);
    if (NULL == channel) {
        printf("%s: ERROR. add channel failed\n", __func__);
        return -MM_CAMERA_E_GENERAL;
    }
    printf("%s: mm_camera_app_add_rdi_channel: ok\n", __func__);

    rc = lib->mm_app_start_channel(test_obj, channel);
    if (MM_CAMERA_OK != rc) {
        printf("%s: ERROR. start rdi failed rc=%d\n", __func__, rc);
        lib->mm_app_del_channel(test_obj, channel);
        return rc;
    }
    printf("%s: mm_app_start_channel: ok\n", __func__);

    return rc;
}