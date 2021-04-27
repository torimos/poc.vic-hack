#ifndef __MMAPP_H
#define __MMAPP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

//=================================================================================================
#define VIDEO_MAX_PLANES 8
#define MAX_STREAM_NUM_IN_BUNDLE 4
#define MM_CAMERA_MAX_NUM_FRAMES 24

#define ION_IOC_INV_CACHES 0xC0144D01

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
    CAM_WAVELET_DENOISE_YCBCR_PLANE,
    CAM_WAVELET_DENOISE_CBCR_ONLY,
    CAM_WAVELET_DENOISE_STREAMLINE_YCBCR,
    CAM_WAVELET_DENOISE_STREAMLINED_CBCR
} cam_denoise_process_type_t;

typedef enum : uint8_t {
    /* applies to HAL 1 */
    CAM_STREAM_TYPE_DEFAULT,       /* default stream type */
    CAM_STREAM_TYPE_PREVIEW,       /* preview */
    CAM_STREAM_TYPE_POSTVIEW,      /* postview */
    CAM_STREAM_TYPE_SNAPSHOT,      /* snapshot */
    CAM_STREAM_TYPE_VIDEO,         /* video */

    /* applies to HAL 3 */
    CAM_STREAM_TYPE_CALLBACK,      /* app requested callback */
    CAM_STREAM_TYPE_IMPL_DEFINED, /* opaque format: could be display, video enc, ZSL YUV */

    /* applies to both HAL 1 and HAL 3 */
    CAM_STREAM_TYPE_METADATA,      /* meta data */
    CAM_STREAM_TYPE_RAW,           /* raw dump from camif */
    CAM_STREAM_TYPE_OFFLINE_PROC,  /* offline process */
    CAM_STREAM_TYPE_PARM,         /* mct internal stream */
    CAM_STREAM_TYPE_ANALYSIS,     /* analysis stream */
    CAM_STREAM_TYPE_MAX,
} cam_stream_type_t;

typedef enum : uint8_t  {
    CAM_STREAM_BUF_TYPE_MPLANE,  /* Multiplanar Buffer type */
    CAM_STREAM_BUF_TYPE_USERPTR, /* User specific structure pointer*/
    CAM_STREAM_BUF_TYPE_MAX
} cam_stream_buf_type;

typedef enum {
    CAM_FORMAT_JPEG = 0,
    CAM_FORMAT_YUV_420_NV12 = 1,
    CAM_FORMAT_YUV_420_NV21,
    CAM_FORMAT_YUV_420_NV21_ADRENO,
    CAM_FORMAT_YUV_420_YV12,
    CAM_FORMAT_YUV_422_NV16,
    CAM_FORMAT_YUV_422_NV61,
    CAM_FORMAT_YUV_420_NV12_VENUS,

    /* Please note below are the defintions for raw image.
     * Any format other than raw image format should be declared
     * before this line!!!!!!!!!!!!! */

    /* Note: For all raw formats, each scanline needs to be 16 bytes aligned */

    /* Packed YUV/YVU raw format, 16 bpp: 8 bits Y and 8 bits UV.
     * U and V are interleaved with Y: YUYV or YVYV */
    CAM_FORMAT_YUV_RAW_8BIT_YUYV,
    CAM_FORMAT_YUV_RAW_8BIT_YVYU,
    CAM_FORMAT_YUV_RAW_8BIT_UYVY,
    CAM_FORMAT_YUV_RAW_8BIT_VYUY,

    /* QCOM RAW formats where data is packed into 64bit word.
     * 8BPP: 1 64-bit word contains 8 pixels p0 - p7, where p0 is
     *       stored at LSB.
     * 10BPP: 1 64-bit word contains 6 pixels p0 - p5, where most
     *       significant 4 bits are set to 0. P0 is stored at LSB.
     * 12BPP: 1 64-bit word contains 5 pixels p0 - p4, where most
     *       significant 4 bits are set to 0. P0 is stored at LSB. */
    CAM_FORMAT_BAYER_QCOM_RAW_8BPP_GBRG,
    CAM_FORMAT_BAYER_QCOM_RAW_8BPP_GRBG,
    CAM_FORMAT_BAYER_QCOM_RAW_8BPP_RGGB,
    CAM_FORMAT_BAYER_QCOM_RAW_8BPP_BGGR,
    CAM_FORMAT_BAYER_QCOM_RAW_10BPP_GBRG,
    CAM_FORMAT_BAYER_QCOM_RAW_10BPP_GRBG,
    CAM_FORMAT_BAYER_QCOM_RAW_10BPP_RGGB,
    CAM_FORMAT_BAYER_QCOM_RAW_10BPP_BGGR,
    CAM_FORMAT_BAYER_QCOM_RAW_12BPP_GBRG,
    CAM_FORMAT_BAYER_QCOM_RAW_12BPP_GRBG,
    CAM_FORMAT_BAYER_QCOM_RAW_12BPP_RGGB,
    CAM_FORMAT_BAYER_QCOM_RAW_12BPP_BGGR,
    /* MIPI RAW formats based on MIPI CSI-2 specifiction.
     * 8BPP: Each pixel occupies one bytes, starting at LSB.
     *       Output with of image has no restrictons.
     * 10BPP: Four pixels are held in every 5 bytes. The output
     *       with of image must be a multiple of 4 pixels.
     * 12BPP: Two pixels are held in every 3 bytes. The output
     *       width of image must be a multiple of 2 pixels. */
    CAM_FORMAT_BAYER_MIPI_RAW_8BPP_GBRG,
    CAM_FORMAT_BAYER_MIPI_RAW_8BPP_GRBG,
    CAM_FORMAT_BAYER_MIPI_RAW_8BPP_RGGB,
    CAM_FORMAT_BAYER_MIPI_RAW_8BPP_BGGR,
    CAM_FORMAT_BAYER_MIPI_RAW_10BPP_GBRG,
    CAM_FORMAT_BAYER_MIPI_RAW_10BPP_GRBG,
    CAM_FORMAT_BAYER_MIPI_RAW_10BPP_RGGB,
    CAM_FORMAT_BAYER_MIPI_RAW_10BPP_BGGR,
    CAM_FORMAT_BAYER_MIPI_RAW_12BPP_GBRG,
    CAM_FORMAT_BAYER_MIPI_RAW_12BPP_GRBG,
    CAM_FORMAT_BAYER_MIPI_RAW_12BPP_RGGB,
    CAM_FORMAT_BAYER_MIPI_RAW_12BPP_BGGR,
    /* Ideal raw formats where image data has gone through black
     * correction, lens rolloff, demux/channel gain, bad pixel
     * correction, and ABF.
     * Ideal raw formats could output any of QCOM_RAW and MIPI_RAW
     * formats, plus plain8 8bbp, plain16 800, plain16 10bpp, and
     * plain 16 12bpp */
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_BGGR,

    /* generic 8-bit raw */
    CAM_FORMAT_JPEG_RAW_8BIT,
    CAM_FORMAT_META_RAW_8BIT,

    /* QCOM RAW formats where data is packed into 64bit word.
     * 14BPP: 1 64-bit word contains 4 pixels p0 - p3, where most
     *       significant 4 bits are set to 0. P0 is stored at LSB.
     */
    CAM_FORMAT_BAYER_QCOM_RAW_14BPP_GBRG,
    CAM_FORMAT_BAYER_QCOM_RAW_14BPP_GRBG,
    CAM_FORMAT_BAYER_QCOM_RAW_14BPP_RGGB,
    CAM_FORMAT_BAYER_QCOM_RAW_14BPP_BGGR,
    /* MIPI RAW formats based on MIPI CSI-2 specifiction.
     * 14 BPPP: 1st byte: P0 [13:6]
     *          2nd byte: P1 [13:6]
     *          3rd byte: P2 [13:6]
     *          4th byte: P3 [13:6]
     *          5th byte: P0 [5:0]
     *          7th byte: P1 [5:0]
     *          8th byte: P2 [5:0]
     *          9th byte: P3 [5:0]
     */
    CAM_FORMAT_BAYER_MIPI_RAW_14BPP_GBRG,
    CAM_FORMAT_BAYER_MIPI_RAW_14BPP_GRBG,
    CAM_FORMAT_BAYER_MIPI_RAW_14BPP_RGGB,
    CAM_FORMAT_BAYER_MIPI_RAW_14BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_BGGR,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_BGGR,
    /* 14BPP: 1st byte: P0 [8:0]
     *        2nd byte: P0 [13:9]
     *        3rd byte: P1 [8:0]
     *        4th byte: P1 [13:9]
     */
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_GBRG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_GRBG,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_RGGB,
    CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_BGGR,

    CAM_FORMAT_YUV_444_NV24,
    CAM_FORMAT_YUV_444_NV42,

    CAM_FORMAT_MAX
} cam_format_t;

typedef enum {
    CAM_FOCUS_MODE_OFF,
    CAM_FOCUS_MODE_AUTO,
    CAM_FOCUS_MODE_INFINITY,
    CAM_FOCUS_MODE_MACRO,
    CAM_FOCUS_MODE_FIXED,
    CAM_FOCUS_MODE_EDOF,
    CAM_FOCUS_MODE_CONTINOUS_VIDEO,
    CAM_FOCUS_MODE_CONTINOUS_PICTURE,
    CAM_FOCUS_MODE_MANUAL,
    CAM_FOCUS_MODE_MAX
} cam_focus_mode_type;

typedef void mm_jpeg_encode_params_t;
typedef void mm_jpeg_job_t;
typedef void mm_camera_ops_t;
typedef void metadata_buffer_t;
typedef metadata_buffer_t parm_buffer_t;

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

typedef struct{
    uint8_t data[8*4];
} cam_mp_len_offset_t;

typedef struct {
    uint8_t data[233*4];
} cam_stream_info_t;

typedef struct {
    uint8_t data[6*4];
} tuningserver_t;

typedef struct {
    uint32_t camera_handle;
    mm_camera_ops_t *ops;
} mm_camera_vtbl_t;

typedef struct {
  uint32_t w;
  uint32_t h;
} mm_dimension;

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
  void *ptr;
  void* ptr_jpeg;

  uint8_t (*get_num_of_cameras) ();
  mm_camera_vtbl_t *(*mm_camera_open) (uint8_t camera_idx);
  uint32_t (*jpeg_open) (mm_jpeg_ops_t *ops, mm_dimension picture_size);

} hal_interface_lib_t;

typedef struct {
    uint8_t num_cameras;
    hal_interface_lib_t hal_lib;
} mm_camera_app_t;

typedef struct {
    uint16_t user_input_display_width;
    uint16_t user_input_display_height;
} USER_INPUT_DISPLAY_T;

typedef struct {
    uint8_t denoise_enable;
    cam_denoise_process_type_t process_plates;
} cam_denoise_param_t;

struct v4l2_plane {
    // uint8_t data[15*4];
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
//     uint8_t data[7*4];
} mm_camera_super_buf_t;

typedef void (*mm_camera_buf_notify_t) (mm_camera_super_buf_t *bufs,
                                        void *user_data);

typedef void (*prev_callback) (mm_camera_buf_def_t *preview_frame);

typedef int ion_user_handle_t;

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
    uint32_t width_padding;
    uint32_t height_padding;
    uint32_t plane_padding;
} cam_padding_info_t;

typedef struct {
    uint32_t num_planes;
    cam_mp_len_offset_t mp[VIDEO_MAX_PLANES];
    uint32_t frame_len;
} cam_frame_len_offset_t;

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
    uint32_t ch_id;
    uint8_t num_streams;
    mm_camera_stream_t streams[MAX_STREAM_NUM_IN_BUNDLE];
} mm_camera_channel_t;

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