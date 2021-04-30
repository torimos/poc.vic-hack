#ifndef __MMAPP_H
#define __MMAPP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "cam_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//=================================================================================================
#define MAX_STREAM_NUM_IN_BUNDLE 4
#define MM_CAMERA_MAX_NUM_FRAMES 24

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
    MM_CAMERA_E_CAPTURE_TIMEOUT, /* 10 */
} mm_camera_status_type_t;

typedef enum {
    MM_CHANNEL_TYPE_ZSL,      /* preview, and snapshot main */
    MM_CHANNEL_TYPE_CAPTURE,  /* snapshot main, and postview */
    MM_CHANNEL_TYPE_PREVIEW,  /* preview only */
    MM_CHANNEL_TYPE_SNAPSHOT, /* snapshot main only */
    MM_CHANNEL_TYPE_VIDEO,    /* video only */
    MM_CHANNEL_TYPE_RDI,      /* rdi only */
    MM_CHANNEL_TYPE_REPROCESS,/* offline reprocess */
    MM_CHANNEL_TYPE_MAX
} mm_camera_channel_type_t;

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

typedef enum {
   MM_CAMERA_AF_BRACKETING = 0,
   MM_CAMERA_AE_BRACKETING,
   MM_CAMERA_FLASH_BRACKETING,
   MM_CAMERA_ZOOM_1X,
   MM_CAMERA_FRAME_CAPTURE,
} mm_camera_advanced_capture_t;

typedef struct {
    uint8_t data[21157*4];
} tune_chromatix_t;

typedef struct {
    uint8_t data[12*4];
} mm_camera_queue_t;

struct mdp_overlay {
    uint8_t data[255*4];
};

struct fb_var_screeninfo {
    uint8_t data[40*4];
};

typedef struct {
    uint8_t data[6*4];
} tuningserver_t;

typedef void mm_jpeg_encode_params_t;
typedef void mm_jpeg_job_t;
typedef void metadata_buffer_t;
typedef metadata_buffer_t parm_buffer_t;
typedef void mm_camera_map_unmap_ops_tbl_t;

typedef int ion_user_handle_t;

typedef struct {
    cam_event_type_t server_event_type;
    uint32_t status;
} mm_camera_event_t;

typedef struct {
  uint32_t w;
  uint32_t h;
} mm_dimension;

typedef struct {
    mm_camera_super_buf_notify_mode_t notify_mode;
    uint8_t water_mark;
    uint8_t look_back;
    uint8_t post_frame_skip;
    uint8_t max_unmatched_frames;
    mm_camera_super_buf_priority_t priority;
} mm_camera_channel_attr_t;

typedef struct {
  /* config a job -- async call */
  int (*start_job)(mm_jpeg_job_t* job, uint32_t* job_id);

  /* abort a job -- sync call */
  int (*abort_job)(uint32_t job_id);

  /* create a session */
  int (*create_session)(uint32_t client_hdl,
    mm_jpeg_encode_params_t *p_params, uint32_t *p_session_id);

  /* destroy session */
  int (*destroy_session)(uint32_t session_id);

  /* close a jpeg client -- sync call */
  int (*close) (uint32_t clientHdl);
} mm_jpeg_ops_t;

typedef struct {
    uint16_t user_input_display_width;
    uint16_t user_input_display_height;
} USER_INPUT_DISPLAY_T;

struct v4l2_plane {
    uint32_t bytesused;
    uint32_t length;
    uint32_t userptr;
    uint32_t data_offset;
    uint32_t reserved[11];
};

typedef struct {
    int8_t num_planes;
    struct v4l2_plane planes[VIDEO_MAX_PLANES];
} mm_camera_plane_buf_def_t;

typedef struct mm_camera_buf_def {
    uint32_t stream_id;
    cam_stream_type_t stream_type;
    cam_stream_buf_type buf_type;
    uint32_t buf_idx;
    uint8_t is_uv_subsampled;
    struct timespec ts;
    uint32_t frame_idx;
    mm_camera_plane_buf_def_t planes_buf;
    int fd;
    void *buffer;
    size_t frame_len;
    void *mem_info;
} mm_camera_buf_def_t;

typedef struct {
    uint32_t camera_handle;
    uint32_t ch_id;
    uint32_t num_bufs;
    mm_camera_buf_def_t* bufs[MAX_STREAM_NUM_IN_BUNDLE];
} mm_camera_super_buf_t;

typedef struct {
    int                     fd;
    int                     main_ion_fd;
    ion_user_handle_t       handle;
    size_t                  size;
    parm_buffer_t          *data;
} mm_camera_app_meminfo_t;

typedef struct {
    mm_camera_buf_def_t buf;
    mm_camera_app_meminfo_t mem_info;
} mm_camera_app_buf_t;

typedef struct {
  void *user_data;
  int32_t (*get_bufs) (cam_frame_len_offset_t *offset,
                       uint8_t *num_bufs,
                       uint8_t **initial_reg_flag,
                       mm_camera_buf_def_t **bufs,
                       void *ops_tbl, //mm_camera_map_unmap_ops_tbl_t
                       void *user_data);
  int32_t (*put_bufs) (void *ops_tbl, //mm_camera_map_unmap_ops_tbl_t
                       void *user_data);
  int32_t (*invalidate_buf)(uint32_t index, void *user_data);
  int32_t (*clean_invalidate_buf)(uint32_t index, void *user_data);
} mm_camera_stream_mem_vtbl_t;

typedef void (*mm_camera_buf_notify_t) (mm_camera_super_buf_t *bufs, void *user_data);
typedef void (*mm_camera_event_notify_t)(uint32_t camera_handle, mm_camera_event_t *evt, void *user_data);

typedef struct {
    cam_stream_info_t *stream_info;
    cam_padding_info_t padding_info;
    mm_camera_stream_mem_vtbl_t mem_vtbl;
    mm_camera_buf_notify_t stream_cb;
    void *userdata;
} mm_camera_stream_config_t;

typedef struct {
    uint32_t s_id;
    mm_camera_stream_config_t s_config;
    cam_frame_len_offset_t offset;
    uint8_t num_of_bufs;
    uint32_t multipleOf;
    mm_camera_app_buf_t s_bufs[MM_CAMERA_MAX_NUM_FRAMES];
    mm_camera_app_buf_t s_info_buf;
} mm_camera_stream_t;

typedef struct {
    /** query_capability: fucntion definition for querying static
     *                    camera capabilities
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume cam_capability_t is already mapped
     **/
    int32_t (*query_capability) (uint32_t camera_handle);

    /** register_event_notify: fucntion definition for registering
     *                         for event notification
     *    @camera_handle : camer handler
     *    @evt_cb : callback for event notify
     *    @user_data : user data poiner
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*register_event_notify) (uint32_t camera_handle,
                                      mm_camera_event_notify_t evt_cb,
                                      void *user_data);

    /** close_camera: fucntion definition for closing a camera
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*close_camera) (uint32_t camera_handle);

    /** map_buf: fucntion definition for mapping a camera buffer
     *           via domain socket
     *    @camera_handle : camer handler
     *    @buf_type : type of mapping buffers, can be value of
     *                CAM_MAPPING_BUF_TYPE_CAPABILITY
     *                CAM_MAPPING_BUF_TYPE_SETPARM_BUF
     *                CAM_MAPPING_BUF_TYPE_GETPARM_BUF
     *    @fd : file descriptor of the stream buffer
     *    @size :  size of the stream buffer
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*map_buf) (uint32_t camera_handle,
                        uint8_t buf_type,
                        int fd,
                        size_t size);

    /** unmap_buf: fucntion definition for unmapping a camera buffer
     *           via domain socket
     *    @camera_handle : camer handler
     *    @buf_type : type of mapping buffers, can be value of
     *                CAM_MAPPING_BUF_TYPE_CAPABILITY
     *                CAM_MAPPING_BUF_TYPE_SETPARM_BUF
     *                CAM_MAPPING_BUF_TYPE_GETPARM_BUF
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*unmap_buf) (uint32_t camera_handle,
                          uint8_t buf_type);

    /** set_parms: fucntion definition for setting camera
     *             based parameters to server
     *    @camera_handle : camer handler
     *    @parms : batch for parameters to be set, stored in
     *               parm_buffer_t
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm_buffer_t is already mapped, and
     *       according parameter entries to be set are filled in the
     *       buf before this call
     **/
    int32_t (*set_parms) (uint32_t camera_handle,
                          parm_buffer_t *parms);

    /** get_parms: fucntion definition for querying camera
     *             based parameters from server
     *    @camera_handle : camer handler
     *    @parms : batch for parameters to be queried, stored in
     *               parm_buffer_t
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm_buffer_t is already mapped, and
     *       according parameter entries to be queried are filled in
     *       the buf before this call
     **/
    int32_t (*get_parms) (uint32_t camera_handle,
                          parm_buffer_t *parms);

    /** do_auto_focus: fucntion definition for performing auto focus
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: if this call success, we will always assume there will
     *        be an auto_focus event following up.
     **/
    int32_t (*do_auto_focus) (uint32_t camera_handle);

    /** cancel_auto_focus: fucntion definition for cancelling
     *                     previous auto focus request
     *    @camera_handle : camer handler
    *  Return value: 0 -- success
    *                -1 -- failure
     **/
    int32_t (*cancel_auto_focus) (uint32_t camera_handle);

    /** prepare_snapshot: fucntion definition for preparing hardware
     *                    for snapshot.
     *    @camera_handle : camer handler
     *    @do_af_flag    : flag indicating if AF needs to be done
     *                     0 -- no AF needed
     *                     1 -- AF needed
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*prepare_snapshot) (uint32_t camera_handle,
                                 int32_t do_af_flag);

    /** start_zsl_snapshot: function definition for starting
     *                    zsl snapshot.
     *    @camera_handle : camer handler
     *    @ch_id         : channel id
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*start_zsl_snapshot) (uint32_t camera_handle, uint32_t ch_id);

    /** stop_zsl_snapshot: function definition for stopping
     *                    zsl snapshot.
     *    @camera_handle : camer handler
     *    @ch_id         : channel id
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*stop_zsl_snapshot) (uint32_t camera_handle, uint32_t ch_id);

    /** add_channel: fucntion definition for adding a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @attr : pointer to channel attribute structure
     *    @channel_cb : callbak to handle bundled super buffer
     *    @userdata : user data pointer
     *  Return value: channel id, zero is invalid ch_id
     * Note: attr, channel_cb, and userdata can be NULL if no
     *       superbufCB is needed
     **/
    uint32_t (*add_channel) (uint32_t camera_handle,
                             mm_camera_channel_attr_t *attr,
                             mm_camera_buf_notify_t channel_cb,
                             void *userdata);

    /** delete_channel: fucntion definition for deleting a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*delete_channel) (uint32_t camera_handle,
                               uint32_t ch_id);

    /** get_bundle_info: function definition for querying bundle
     *  info of the channel
     *    @camera_handle : camera handler
     *    @ch_id         : channel handler
     *    @bundle_info   : bundle info to be filled in
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*get_bundle_info) (uint32_t camera_handle,
                                uint32_t ch_id,
                                cam_bundle_config_t *bundle_info);

    /** add_stream: fucntion definition for adding a stream
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: stream_id. zero is invalid stream_id
     **/
    uint32_t (*add_stream) (uint32_t camera_handle,
                            uint32_t ch_id);

    /** delete_stream: fucntion definition for deleting a stream
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*delete_stream) (uint32_t camera_handle,
                              uint32_t ch_id,
                              uint32_t stream_id);

    /** link_stream: function definition for linking a stream
     *    @camera_handle : camera handle
     *    @ch_id : channel handle from which the stream originates
     *    @stream_id : stream handle
     *    @linked_ch_id: channel handle in which the stream will be linked
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*link_stream) (uint32_t camera_handle,
          uint32_t ch_id,
          uint32_t stream_id,
          uint32_t linked_ch_id);

    /** config_stream: fucntion definition for configuring a stream
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @confid : pointer to a stream configuration structure
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*config_stream) (uint32_t camera_handle,
                              uint32_t ch_id,
                              uint32_t stream_id,
                              mm_camera_stream_config_t *config);

    /** map_stream_buf: fucntion definition for mapping
     *                 stream buffer via domain socket
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @buf_type : type of mapping buffers, can be value of
     *             CAM_MAPPING_BUF_TYPE_STREAM_BUF
     *             CAM_MAPPING_BUF_TYPE_STREAM_INFO
     *             CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
     *    @buf_idx : buffer index within the stream buffers
     *    @plane_idx : plane index. If all planes share the same fd,
     *               plane_idx = -1; otherwise, plean_idx is the
     *               index to plane (0..num_of_planes)
     *    @fd : file descriptor of the stream buffer
     *    @size :  size of the stream buffer
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*map_stream_buf) (uint32_t camera_handle,
                               uint32_t ch_id,
                               uint32_t stream_id,
                               uint8_t buf_type,
                               uint32_t buf_idx,
                               int32_t plane_idx,
                               int fd,
                               size_t size);

    /** unmap_stream_buf: fucntion definition for unmapping
     *                 stream buffer via domain socket
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @buf_type : type of mapping buffers, can be value of
     *             CAM_MAPPING_BUF_TYPE_STREAM_BUF
     *             CAM_MAPPING_BUF_TYPE_STREAM_INFO
     *             CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
     *    @buf_idx : buffer index within the stream buffers
     *    @plane_idx : plane index. If all planes share the same fd,
     *               plane_idx = -1; otherwise, plean_idx is the
     *               index to plane (0..num_of_planes)
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*unmap_stream_buf) (uint32_t camera_handle,
                                 uint32_t ch_id,
                                 uint32_t stream_id,
                                 uint8_t buf_type,
                                 uint32_t buf_idx,
                                 int32_t plane_idx);

    /** set_stream_parms: fucntion definition for setting stream
     *                    specific parameters to server
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @parms : batch for parameters to be set
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm buffer is already mapped, and
     *       according parameter entries to be set are filled in the
     *       buf before this call
     **/
    int32_t (*set_stream_parms) (uint32_t camera_handle,
                                 uint32_t ch_id,
                                 uint32_t s_id,
                                 cam_stream_parm_buffer_t *parms);

    /** get_stream_parms: fucntion definition for querying stream
     *                    specific parameters from server
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @parms : batch for parameters to be queried
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm buffer is already mapped, and
     *       according parameter entries to be queried are filled in
     *       the buf before this call
     **/
    int32_t (*get_stream_parms) (uint32_t camera_handle,
                                 uint32_t ch_id,
                                 uint32_t s_id,
                                 cam_stream_parm_buffer_t *parms);

    /** start_channel: fucntion definition for starting a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     * This call will start all streams belongs to the channel
     **/
    int32_t (*start_channel) (uint32_t camera_handle,
                              uint32_t ch_id);

    /** stop_channel: fucntion definition for stopping a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     * This call will stop all streams belongs to the channel
     **/
    int32_t (*stop_channel) (uint32_t camera_handle,
                             uint32_t ch_id);

    /** qbuf: fucntion definition for queuing a frame buffer back to
     *        kernel for reuse
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @buf : a frame buffer to be queued back to kernel
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*qbuf) (uint32_t camera_handle,
                     uint32_t ch_id,
                     mm_camera_buf_def_t *buf);

    /** get_queued_buf_count: fucntion definition for querying queued buf count
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *  Return value: queued buf count
     **/
    int32_t (*get_queued_buf_count) (uint32_t camera_handle,
            uint32_t ch_id,
            uint32_t stream_id);

    /** request_super_buf: fucntion definition for requesting frames
     *                     from superbuf queue in burst mode
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @num_buf_requested : number of super buffers requested
     *    @num_retro_buf_requested : number of retro buffers requested
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*request_super_buf) (uint32_t camera_handle,
                                  uint32_t ch_id,
                                  uint32_t num_buf_requested,
                                  uint32_t num_retro_buf_requested);

    /** cancel_super_buf_request: fucntion definition for canceling
     *                     frames dispatched from superbuf queue in
     *                     burst mode
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*cancel_super_buf_request) (uint32_t camera_handle,
                                         uint32_t ch_id);

    /** flush_super_buf_queue: function definition for flushing out
     *                     all frames in the superbuf queue up to frame_idx,
     *                     even if frames with frame_idx come in later than
     *                     this call.
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @frame_idx : frame index up until which all superbufs are flushed
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*flush_super_buf_queue) (uint32_t camera_handle,
                                      uint32_t ch_id, uint32_t frame_idx);

    /** configure_notify_mode: function definition for configuring the
     *                         notification mode of channel
     *    @camera_handle : camera handler
     *    @ch_id : channel handler
     *    @notify_mode : notification mode
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*configure_notify_mode) (uint32_t camera_handle,
                                      uint32_t ch_id,
                                      mm_camera_super_buf_notify_mode_t notify_mode);

   /** process_advanced_capture: function definition for start/stop advanced capture
     *                    for snapshot.
     *    @camera_handle : camera handle
     *    @ch_id : channel handler
     *    @type :  advanced capture type.
     *    @trigger    : flag indicating if advanced capture needs to be done
     *                     0 -- stop advanced capture
     *                     1 -- start advanced capture
     *    @in_value: Input value. Configaration
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*process_advanced_capture) (uint32_t camera_handle,
             uint32_t ch_id, mm_camera_advanced_capture_t type,
             int8_t start_flag, void *in_value);
} mm_camera_ops_t;

typedef struct {
    uint32_t camera_handle;
    mm_camera_ops_t *ops;
} mm_camera_vtbl_t;

typedef struct {
    uint32_t ch_id;
    uint8_t num_streams;
    mm_camera_stream_t streams[MAX_STREAM_NUM_IN_BUNDLE];
} mm_camera_channel_t;

typedef void (*prev_callback) (mm_camera_buf_def_t *preview_frame);

typedef struct {
    mm_camera_vtbl_t *cam;
    uint8_t num_channels;
    mm_camera_channel_t channels[MM_CHANNEL_TYPE_MAX];
    mm_jpeg_ops_t jpeg_ops;
    uint32_t jpeg_hdl;
    mm_camera_app_buf_t cap_buf;
    mm_camera_app_buf_t parm_buf;

    uint32_t current_jpeg_sess_id;
    mm_camera_super_buf_t* current_job_frames;
    uint32_t current_job_id;
    mm_camera_app_buf_t jpeg_buf;

    int fb_fd;
    struct fb_var_screeninfo vinfo;
    struct mdp_overlay data_overlay;
    uint32_t slice_size;
    uint32_t buffer_width, buffer_height;
    uint32_t buffer_size;
    cam_format_t buffer_format;
    uint32_t frame_size;
    uint32_t frame_count;
    int encodeJpeg;
    int zsl_enabled;
    int8_t focus_supported;
    prev_callback user_preview_cb;
    uint32_t ___unkdata1;
    uint32_t ___unkdata2;
    parm_buffer_t *params_buffer;
    USER_INPUT_DISPLAY_T preview_resolution;

    //Reprocess params&stream
    int8_t enable_reproc;
    int32_t reproc_sharpness;
    cam_denoise_param_t reproc_wnr;
    int8_t enable_CAC;
    mm_camera_queue_t pp_frames;
    mm_camera_stream_t *reproc_stream;
    metadata_buffer_t *metadata;
    int8_t is_chromatix_reload;
    tune_chromatix_t tune_data;
} mm_camera_test_obj_t;

typedef struct {
    int32_t stream_width, stream_height;
    cam_focus_mode_type af_mode;
} mm_camera_lib_params;

typedef struct {
  void *ptr;
  void *ptr_jpeg;

  uint8_t (*get_num_of_cameras) ();
  mm_camera_vtbl_t *(*mm_camera_open) (uint8_t camera_idx);
  uint32_t (*jpeg_open) (mm_jpeg_ops_t *ops, mm_dimension picture_size);

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

#ifdef __cplusplus
}
#endif


#endif