#ifndef __MMAPP_H
#define __MMAPP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #include "mm_camera_interface.h"
// #include "mm_qcamera_socket.h"
// #include <msm/msm_mdp.h>

#include "cam_types.h"
#include "cam_list.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STREAM_NUM_IN_BUNDLE 6

typedef enum {
    MM_CAMERA_OK,
    MM_CAMERA_E_GENERAL,
    MM_CAMERA_E_NO_MEMORY,
    MM_CAMERA_E_NOT_SUPPORTED,
    MM_CAMERA_E_INVALID_INPUT,
    MM_CAMERA_E_INVALID_OPERATION, /* 5 */
    MM_CAMERA_E_ENCODE,
    MM_CAMERA_E_BUFFER_REG,
    MM_CAMERA_E_PMEM_ALLOC,
    MM_CAMERA_E_CAPTURE_FAILED,
    MM_CAMERA_E_CAPTURE_TIMEOUT /* 10 */
} mm_camera_status_type_t;

typedef enum {
    MM_CHANNEL_TYPE_ZSL,      /* preview, and snapshot main */
    MM_CHANNEL_TYPE_CAPTURE,  /* snapshot main, and postview */
    MM_CHANNEL_TYPE_PREVIEW,  /* preview only */
    MM_CHANNEL_TYPE_SNAPSHOT, /* snapshot main only */
    MM_CHANNEL_TYPE_VIDEO,    /* video only */
    MM_CHANNEL_TYPE_RDI,      /* rdi only */
    // MM_CHANNEL_TYPE_REPROCESS, /* offline reprocess */
    MM_CHANNEL_TYPE_MAX
} mm_camera_channel_type_t;

typedef void (*release_data_fn)(void* data, void *user_data);

typedef struct {
    struct cam_list list;
    void* data;
} camera_q_node;

typedef struct {
    camera_q_node m_head;
    int m_size;
    pthread_mutex_t m_lock;
    release_data_fn m_dataFn;
    void * m_userData;
} mm_camera_queue_t;

typedef struct {
    void *command_process;
    void * prevcommand_process;
    void *prevframe_callback;
} mm_camera_tune_func_t;

typedef struct {
    mm_camera_tune_func_t *func_tbl;
    void *lib_handle;
} mm_camera_tuning_lib_params_t;

typedef struct {
  void *proto;
  int clientsocket_id;
  void *pr_proto;
  int pr_clientsocket_id;
  mm_camera_tuning_lib_params_t tuning_params;
} tuningserver_t;

typedef struct {
    int32_t stream_width, stream_height;
    cam_focus_mode_type af_mode;
} mm_camera_lib_params;

typedef struct {
    uint32_t camera_handle;
    void *ops;//mm_camera_ops_t
} mm_camera_vtbl_t;

typedef struct {
    // uint32_t s_id;
    // mm_camera_stream_config_t s_config;
    // cam_frame_len_offset_t offset;
    // uint8_t num_of_bufs;
    // uint32_t multipleOf;
    // mm_camera_app_buf_t s_bufs[MM_CAMERA_MAX_NUM_FRAMES];
    // mm_camera_app_buf_t s_info_buf;
    uint8_t data[10905];
} mm_camera_stream_t;

typedef struct {
    uint32_t ch_id;
    uint8_t num_streams;
    mm_camera_stream_t streams[MAX_STREAM_NUM_IN_BUNDLE];
} mm_camera_channel_t;

typedef struct {
    uint16_t user_input_display_width;
    uint16_t user_input_display_height;
} USER_INPUT_DISPLAY_T;


typedef int ion_user_handle_t;
typedef struct {
    int                     fd;
    int                     main_ion_fd;
    ion_user_handle_t       handle;
    size_t                  size;
    void                    *data;//parm_buffer_t
} mm_camera_app_meminfo_t;
typedef struct {
    uint8_t buf[528];
    mm_camera_app_meminfo_t mem_info;
} mm_camera_app_buf_t;

typedef struct {
    int* ops[5];
} mm_jpeg_ops_t;

typedef struct {
    mm_camera_vtbl_t *cam;
    uint8_t num_channels;
    mm_camera_channel_t channels[MM_CHANNEL_TYPE_MAX];
    mm_jpeg_ops_t jpeg_ops;
    uint32_t jpeg_hdl;
    mm_camera_app_buf_t cap_buf;
    mm_camera_app_buf_t parm_buf;

    uint32_t current_jpeg_sess_id;
    void* current_job_frames; //mm_camera_super_buf_t
    uint32_t current_job_id;
    mm_camera_app_buf_t jpeg_buf;
    
    int fb_fd;
    uint8_t vinfo[160];//struct fb_var_screeninfo 
    uint8_t data_overlay[1015+4];// struct mdp_overlay
    uint32_t slice_size;
    
    uint32_t buffer_width, buffer_height;
    uint32_t buffer_size;
    cam_format_t buffer_format;

    uint32_t frame_size;
    uint32_t frame_count;
    int encodeJpeg;
    int zsl_enabled;
    int8_t focus_supported;
uint8_t ___unknown_data2[11];
    void *user_preview_cb; //prev_callback
    void *params_buffer; //parm_buffer_t
uint8_t ___unknown_data3[84718-4*2]; //tune_chromatix_t
} mm_camera_test_obj_t;
//480256

typedef struct {
  uint32_t w;
  uint32_t h;
} mm_dimension;

typedef struct {
  void *ptr;
  void* ptr_jpeg;

  uint8_t (*get_num_of_cameras) ();
  void *(*mm_camera_open) (uint8_t camera_idx);
  uint32_t (*jpeg_open) (void *ops, mm_dimension picture_size);
} hal_interface_lib_t;

typedef struct {
    uint8_t num_cameras;
    hal_interface_lib_t hal_lib;
} mm_camera_app_t;

typedef struct {
    mm_camera_app_t app_ctx;
    mm_camera_test_obj_t test_obj;
    mm_camera_lib_params current_params;
    int stream_running;
    tuningserver_t tsctrl;
} mm_camera_lib_handle;
//480320

typedef enum {
    MM_CAMERA_SUPER_BUF_NOTIFY_BURST = 0,
    MM_CAMERA_SUPER_BUF_NOTIFY_CONTINUOUS,
    MM_CAMERA_SUPER_BUF_NOTIFY_MAX
} mm_camera_super_buf_notify_mode_t;
typedef enum {
    MM_CAMERA_SUPER_BUF_PRIORITY_NORMAL = 0,
    MM_CAMERA_SUPER_BUF_PRIORITY_FOCUS,
    MM_CAMERA_SUPER_BUF_PRIORITY_EXPOSURE_BRACKETING,
    MM_CAMERA_SUPER_BUF_PRIORITY_LOW,/* Bundled metadata frame may not match*/
    MM_CAMERA_SUPER_BUF_PRIORITY_MAX
} mm_camera_super_buf_priority_t;
typedef struct {
    mm_camera_super_buf_notify_mode_t notify_mode;
    uint8_t water_mark;
    uint8_t look_back;
    uint8_t post_frame_skip;
    uint8_t max_unmatched_frames;
    mm_camera_super_buf_priority_t priority;
} mm_camera_channel_attr_t;

#ifdef __cplusplus
}
#endif


#endif