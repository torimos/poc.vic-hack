#ifndef __CAM_H
#define __CAM_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mm_app.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    mm_camera_lib_handle handle;

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
    int (*mm_app_config_stream) (mm_camera_test_obj_t *test_obj, mm_camera_channel_t *channel, mm_camera_stream_t *stream, mm_camera_stream_config_t *config);
    int (*mm_app_start_rdi) (mm_camera_test_obj_t *test_obj, uint8_t num_burst, mm_camera_buf_notify_t notify_cb);
    int (*mm_app_cache_ops) (mm_camera_app_meminfo_t *mem_info,int cmd);
    int (*mm_app_stream_initbuf) (cam_frame_len_offset_t *frame_offset_info,
                            uint8_t *num_bufs,
                            uint8_t **initial_reg_flag,
                            mm_camera_buf_def_t **bufs,
                            mm_camera_map_unmap_ops_tbl_t *ops_tbl,
                            void *user_data);
    int32_t (*mm_app_stream_deinitbuf) (mm_camera_map_unmap_ops_tbl_t *ops_tbl, void *user_data);
    int32_t (*mm_app_stream_clean_invalidate_buf) (uint32_t index, void *user_data);
    int32_t (*mm_app_stream_invalidate_buf) (uint32_t index, void *user_data);
} mm_cam_lib_t;

int lib_init(mm_cam_lib_t* handle);
int mm_camera_app_wait();
void mm_camera_app_done();
int mm_camera_app_start(mm_cam_lib_t *lib, uint8_t num_burst, mm_camera_buf_notify_t notify_cb);

#ifdef __cplusplus
}
#endif


#endif