#ifndef __CAM_TYPES_H
#define __CAM_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

//=================================================================================================
#define VIDEO_MAX_PLANES 8

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

typedef enum {
    CAM_EVENT_TYPE_MAP_UNMAP_DONE  = (1<<0),
    CAM_EVENT_TYPE_AUTO_FOCUS_DONE = (1<<1),
    CAM_EVENT_TYPE_ZOOM_DONE       = (1<<2),
    CAM_EVENT_TYPE_DAEMON_DIED     = (1<<3),
    CAM_EVENT_TYPE_INT_TAKE_JPEG   = (1<<4),
    CAM_EVENT_TYPE_INT_TAKE_RAW    = (1<<5),
    CAM_EVENT_TYPE_DAEMON_PULL_REQ = (1<<6),
    CAM_EVENT_TYPE_MAX
} cam_event_type_t;
typedef enum {
    CAM_INTF_PARM_HAL_VERSION,

    /* Overall mode of 3A control routines. We need to have this parameter
     * because not all android.control.* have an OFF option, for example,
     * AE_FPS_Range, aePrecaptureTrigger */
    CAM_INTF_META_MODE,
    /* Whether AE is currently updating the sensor exposure and sensitivity
     * fields */
    CAM_INTF_META_AEC_MODE,
    CAM_INTF_PARM_WHITE_BALANCE,
    CAM_INTF_PARM_FOCUS_MODE,

    /* common between HAL1 and HAL3 */
    CAM_INTF_PARM_ANTIBANDING,
    CAM_INTF_PARM_EXPOSURE_COMPENSATION,
    CAM_INTF_PARM_EV_STEP,
    CAM_INTF_PARM_AEC_LOCK,
    CAM_INTF_PARM_FPS_RANGE,
    CAM_INTF_PARM_AWB_LOCK, /* 10 */
    CAM_INTF_PARM_EFFECT,
    CAM_INTF_PARM_BESTSHOT_MODE,
    CAM_INTF_PARM_DIS_ENABLE,
    CAM_INTF_PARM_LED_MODE,
    CAM_INTF_META_HISTOGRAM,
    CAM_INTF_META_FACE_DETECTION,
    /* Whether optical image stabilization is enabled. */
    CAM_INTF_META_LENS_OPT_STAB_MODE,

    /* specific to HAl1 */
    CAM_INTF_META_AUTOFOCUS_DATA,
    CAM_INTF_PARM_QUERY_FLASH4SNAP,
    CAM_INTF_PARM_EXPOSURE, /* 20 */
    CAM_INTF_PARM_SHARPNESS,
    CAM_INTF_PARM_CONTRAST,
    CAM_INTF_PARM_SATURATION,
    CAM_INTF_PARM_BRIGHTNESS,
    CAM_INTF_PARM_ISO,
    CAM_INTF_PARM_ZOOM,
    CAM_INTF_PARM_ROLLOFF,
    CAM_INTF_PARM_MODE,             /* camera mode */
    CAM_INTF_PARM_AEC_ALGO_TYPE,    /* auto exposure algorithm */
    CAM_INTF_PARM_FOCUS_ALGO_TYPE, /* 30 */ /* focus algorithm */
    CAM_INTF_PARM_AEC_ROI,
    CAM_INTF_PARM_AF_ROI,
    CAM_INTF_PARM_SCE_FACTOR,
    CAM_INTF_PARM_FD,
    CAM_INTF_PARM_MCE,
    CAM_INTF_PARM_HFR,
    CAM_INTF_PARM_REDEYE_REDUCTION,
    CAM_INTF_PARM_WAVELET_DENOISE,
    CAM_INTF_PARM_TEMPORAL_DENOISE,
    CAM_INTF_PARM_HISTOGRAM, /* 40 */
    CAM_INTF_PARM_ASD_ENABLE,
    CAM_INTF_PARM_RECORDING_HINT,
    CAM_INTF_PARM_HDR,
    CAM_INTF_PARM_MAX_DIMENSION,
    CAM_INTF_PARM_RAW_DIMENSION,
    CAM_INTF_PARM_FRAMESKIP,
    CAM_INTF_PARM_ZSL_MODE,  /* indicating if it's running in ZSL mode */
    CAM_INTF_PARM_BURST_NUM,
    CAM_INTF_PARM_RETRO_BURST_NUM,
    CAM_INTF_PARM_BURST_LED_ON_PERIOD, /* 50 */
    CAM_INTF_PARM_HDR_NEED_1X, /* if HDR needs 1x output */
    CAM_INTF_PARM_LOCK_CAF,
    CAM_INTF_PARM_VIDEO_HDR,
    CAM_INTF_PARM_SENSOR_HDR,
    CAM_INTF_PARM_ROTATION,
    CAM_INTF_PARM_SCALE,
    CAM_INTF_PARM_VT, /* indicating if it's a Video Call Apllication */
    CAM_INTF_META_CROP_DATA,
    CAM_INTF_META_PREP_SNAPSHOT_DONE,
    CAM_INTF_META_GOOD_FRAME_IDX_RANGE, /* 60 */
    CAM_INTF_META_ASD_HDR_SCENE_DATA,
    CAM_INTF_META_ASD_SCENE_TYPE,
    CAM_INTF_META_CURRENT_SCENE,
    CAM_INTF_META_AEC_INFO,
    CAM_INTF_META_SENSOR_INFO,
    CAM_INTF_META_ASD_SCENE_CAPTURE_TYPE,
    CAM_INTF_META_CHROMATIX_LITE_ISP,
    CAM_INTF_META_CHROMATIX_LITE_PP,
    CAM_INTF_META_CHROMATIX_LITE_AE,
    CAM_INTF_META_CHROMATIX_LITE_AWB, /* 70 */
    CAM_INTF_META_CHROMATIX_LITE_AF,
    CAM_INTF_META_CHROMATIX_LITE_ASD,
    CAM_INTF_META_EXIF_DEBUG_AE,
    CAM_INTF_META_EXIF_DEBUG_AWB,
    CAM_INTF_META_EXIF_DEBUG_AF,
    CAM_INTF_META_EXIF_DEBUG_ASD,
    CAM_INTF_META_EXIF_DEBUG_STATS,
    CAM_INTF_PARM_GET_CHROMATIX,
    CAM_INTF_PARM_SET_RELOAD_CHROMATIX,
    CAM_INTF_PARM_SET_AUTOFOCUSTUNING, /* 80 */
    CAM_INTF_PARM_GET_AFTUNE,
    CAM_INTF_PARM_SET_RELOAD_AFTUNE,
    CAM_INTF_PARM_SET_VFE_COMMAND,
    CAM_INTF_PARM_SET_PP_COMMAND,
    CAM_INTF_PARM_TINTLESS,
    CAM_INTF_PARM_LONGSHOT_ENABLE,
    CAM_INTF_PARM_RDI_MODE,
    CAM_INTF_PARM_CDS_MODE,
    CAM_INTF_PARM_TONE_MAP_MODE,
    CAM_INTF_BUF_DIVERT_INFO,
    CAM_INTF_PARM_CAPTURE_FRAME_CONFIG,

    /* stream based parameters */
    CAM_INTF_PARM_DO_REPROCESS,
    CAM_INTF_PARM_SET_BUNDLE, /* 90 */
    CAM_INTF_PARM_STREAM_FLIP,
    CAM_INTF_PARM_GET_OUTPUT_CROP,

    CAM_INTF_PARM_EZTUNE_CMD,
    CAM_INTF_PARM_INT_EVT,

    /* specific to HAL3 */
    /* Whether the metadata maps to a valid frame number */
    CAM_INTF_META_FRAME_NUMBER_VALID,
    /* Whether the urgent metadata maps to a valid frame number */
    CAM_INTF_META_URGENT_FRAME_NUMBER_VALID,
    /* Whether the stream buffer corresponding this frame is dropped or not */
    CAM_INTF_META_FRAME_DROPPED,
    /* COLOR CORRECTION.*/
    CAM_INTF_META_COLOR_CORRECT_MODE,
    /* A transform matrix to chromatically adapt pixels in the CIE XYZ (1931)
     * color space from the scene illuminant to the sRGB-standard D65-illuminant. */
    CAM_INTF_META_COLOR_CORRECT_TRANSFORM,
    /*Color channel gains in the Bayer raw domain in the order [RGeGoB]*/
    CAM_INTF_META_COLOR_CORRECT_GAINS, /* 100 */
    /*The best fit color transform matrix calculated by the stats*/
    CAM_INTF_META_PRED_COLOR_CORRECT_TRANSFORM,
    /*The best fit color channels gains calculated by the stats*/
    CAM_INTF_META_PRED_COLOR_CORRECT_GAINS,
    /* CONTROL */
    /* A frame counter set by the framework. Must be maintained unchanged in
     * output frame. */
    CAM_INTF_META_FRAME_NUMBER,
    /* A frame counter set by the framework. Must be maintained unchanged in
     * output frame. */
    CAM_INTF_META_URGENT_FRAME_NUMBER,
    /*Number of streams and size of streams in current configuration*/
    CAM_INTF_META_STREAM_INFO,
    /* List of areas to use for metering */
    CAM_INTF_META_AEC_ROI,
    /* Whether the HAL must trigger precapture metering.*/
    CAM_INTF_META_AEC_PRECAPTURE_TRIGGER,
    /* The ID sent with the latest CAMERA2_TRIGGER_PRECAPTURE_METERING call */
    /* Current state of AE algorithm */
    CAM_INTF_META_AEC_STATE,
    /* List of areas to use for focus estimation */
    CAM_INTF_META_AF_ROI,
    /* Whether the HAL must trigger autofocus. */
    CAM_INTF_META_AF_TRIGGER, /* 110 */
    /* Current state of AF algorithm */
    CAM_INTF_META_AF_STATE,
    /* List of areas to use for illuminant estimation */
    CAM_INTF_META_AWB_REGIONS,
    /* Current state of AWB algorithm */
    CAM_INTF_META_AWB_STATE,
    /*Whether black level compensation is frozen or free to vary*/
    CAM_INTF_META_BLACK_LEVEL_LOCK,
    /* Information to 3A routines about the purpose of this capture, to help
     * decide optimal 3A strategy */
    CAM_INTF_META_CAPTURE_INTENT,
    /* DEMOSAIC */
    /* Controls the quality of the demosaicing processing */
    CAM_INTF_META_DEMOSAIC,
    /* EDGE */
    /* Operation mode for edge enhancement */
    CAM_INTF_META_EDGE_MODE,
    /* Control the amount of edge enhancement applied to the images.*/
    /* 1-10; 10 is maximum sharpening */
    CAM_INTF_META_SHARPNESS_STRENGTH,
    /* FLASH */
    /* Power for flash firing/torch, 10 is max power; 0 is no flash. Linear */
    CAM_INTF_META_FLASH_POWER,
    /* Firing time of flash relative to start of exposure, in nanoseconds*/
    CAM_INTF_META_FLASH_FIRING_TIME, /* 120 */
    /* Current state of the flash unit */
    CAM_INTF_META_FLASH_STATE,
    /* GEOMETRIC */
    /* Operating mode of geometric correction */
    CAM_INTF_META_GEOMETRIC_MODE,
    /* Control the amount of shading correction applied to the images */
    CAM_INTF_META_GEOMETRIC_STRENGTH,
    /* HOT PIXEL */
    /* Set operational mode for hot pixel correction */
    CAM_INTF_META_HOTPIXEL_MODE,
    /* LENS */
    /* Size of the lens aperture */
    CAM_INTF_META_LENS_APERTURE,
    /* State of lens neutral density filter(s) */
    CAM_INTF_META_LENS_FILTERDENSITY,
    /* Lens optical zoom setting */
    CAM_INTF_META_LENS_FOCAL_LENGTH,
    /* Distance to plane of sharpest focus, measured from frontmost surface
     * of the lens */
    CAM_INTF_META_LENS_FOCUS_DISTANCE,
    /* The range of scene distances that are in sharp focus (depth of field) */
    CAM_INTF_META_LENS_FOCUS_RANGE,
    /*Whether the hal needs to output the lens shading map*/
    CAM_INTF_META_LENS_SHADING_MAP_MODE, /* 130 */
    /* Current lens status */
    CAM_INTF_META_LENS_STATE,
    /* NOISE REDUCTION */
    /* Mode of operation for the noise reduction algorithm */
    CAM_INTF_META_NOISE_REDUCTION_MODE,
   /* Control the amount of noise reduction applied to the images.
    * 1-10; 10 is max noise reduction */
    CAM_INTF_META_NOISE_REDUCTION_STRENGTH,
    /* SCALER */
    /* Top-left corner and width of the output region to select from the active
     * pixel array */
    CAM_INTF_META_SCALER_CROP_REGION,
    /* The estimated scene illumination lighting frequency */
    CAM_INTF_META_SCENE_FLICKER,
    /* SENSOR */
    /* Duration each pixel is exposed to light, in nanoseconds */
    CAM_INTF_META_SENSOR_EXPOSURE_TIME,
    /* Duration from start of frame exposure to start of next frame exposure,
     * in nanoseconds */
    CAM_INTF_META_SENSOR_FRAME_DURATION,
    /* Gain applied to image data. Must be implemented through analog gain only
     * if set to values below 'maximum analog sensitivity'. */
    CAM_INTF_META_SENSOR_SENSITIVITY,
    /* Time at start of exposure of first row */
    CAM_INTF_META_SENSOR_TIMESTAMP,
    /* Duration b/w start of first row exposure and the start of last
       row exposure in nanoseconds */
    CAM_INTF_META_SENSOR_ROLLING_SHUTTER_SKEW, /* 140 */
    /* SHADING */
    /* Quality of lens shading correction applied to the image data */
    CAM_INTF_META_SHADING_MODE,
    /* Control the amount of shading correction applied to the images.
     * unitless: 1-10; 10 is full shading compensation */
    CAM_INTF_META_SHADING_STRENGTH,
    /* STATISTICS */
    /* State of the face detector unit */
    CAM_INTF_META_STATS_FACEDETECT_MODE,
    /* Operating mode for histogram generation */
    CAM_INTF_META_STATS_HISTOGRAM_MODE,
    /* Operating mode for sharpness map generation */
    CAM_INTF_META_STATS_SHARPNESS_MAP_MODE,
    /* A 3-channel sharpness map, based on the raw sensor data,
     * If only a monochrome sharpness map is supported, all channels
     * should have the same data
     */
    CAM_INTF_META_STATS_SHARPNESS_MAP,

    /* TONEMAP */
    /* Tone map mode */
    CAM_INTF_META_TONEMAP_MODE,
    /* Table mapping RGB input values to output values */
    CAM_INTF_META_TONEMAP_CURVES,

    CAM_INTF_META_FLASH_MODE,
    /* 2D array of gain factors for each color channel that was used to
     * compensate for lens shading for this frame */
    CAM_INTF_META_LENS_SHADING_MAP, /* 150 */
    CAM_INTF_META_PRIVATE_DATA,
    CAM_INTF_PARM_STATS_DEBUG_MASK,
    CAM_INTF_PARM_STATS_AF_PAAF,
    /* Indicates streams ID of all the requested buffers */
    CAM_INTF_META_STREAM_ID,
    CAM_INTF_PARM_FOCUS_BRACKETING,
    CAM_INTF_PARM_FLASH_BRACKETING,
    CAM_INTF_PARM_GET_IMG_PROP,
    CAM_INTF_META_JPEG_GPS_COORDINATES,
    CAM_INTF_META_JPEG_GPS_PROC_METHODS,
    CAM_INTF_META_JPEG_GPS_TIMESTAMP, /* 160 */
    CAM_INTF_META_JPEG_ORIENTATION,
    CAM_INTF_META_JPEG_QUALITY,
    CAM_INTF_META_JPEG_THUMB_QUALITY,
    CAM_INTF_META_JPEG_THUMB_SIZE,

    CAM_INTF_META_TEST_PATTERN_DATA,
    /* DNG file support */
    CAM_INTF_META_PROFILE_TONE_CURVE,
    CAM_INTF_META_NEUTRAL_COL_POINT,

    /* CAC */
    CAM_INTF_PARM_CAC,

    /* trigger for all modules to read the debug/log level properties */
    CAM_INTF_PARM_UPDATE_DEBUG_LEVEL,

    /* OTP : WB gr/gb */
    CAM_INTF_META_OTP_WB_GRGB, /* 170 */
    /* LED override for EZTUNE */
    CAM_INTF_META_LED_MODE_OVERRIDE,
    /* auto lens position info */
    CAM_INTF_META_FOCUS_POSITION,
    /* Manual exposure time */
    CAM_INTF_PARM_EXPOSURE_TIME,
    /* AWB meta data info */
    CAM_INTF_META_AWB_INFO,
    /* Manual lens position info */
    CAM_INTF_PARM_MANUAL_FOCUS_POS,
    /* Manual White balance gains */
    CAM_INTF_PARM_WB_MANUAL,
    /* IMG LIB reprocess debug section */
    CAM_INTF_META_IMGLIB, /* cam_intf_meta_imglib_t */
    /* FLIP mode parameter*/
    CAM_INTF_PARM_FLIP,
    CAM_INTF_META_USE_AV_TIMER,

    CAM_INTF_META_EFFECTIVE_EXPOSURE_FACTOR,
    CAM_INTF_PARM_MAX
} cam_intf_parm_type_t;

typedef enum {
    CAM_STREAM_PARAM_TYPE_DO_REPROCESS = CAM_INTF_PARM_DO_REPROCESS,
    CAM_STREAM_PARAM_TYPE_SET_BUNDLE_INFO = CAM_INTF_PARM_SET_BUNDLE,
    CAM_STREAM_PARAM_TYPE_SET_FLIP = CAM_INTF_PARM_STREAM_FLIP,
    CAM_STREAM_PARAM_TYPE_GET_OUTPUT_CROP = CAM_INTF_PARM_GET_OUTPUT_CROP,
    CAM_STREAM_PARAM_TYPE_GET_IMG_PROP = CAM_INTF_PARM_GET_IMG_PROP,
    CAM_STREAM_PARAM_TYPE_MAX
} cam_stream_param_type_e;

typedef enum {
    CAM_STREAMING_MODE_CONTINUOUS, /* continous streaming */
    CAM_STREAMING_MODE_BURST,      /* burst streaming */
    CAM_STREAMING_MODE_BATCH,      /* stream frames in batches */
    CAM_STREAMING_MODE_MAX
} cam_streaming_mode_t;

typedef void cam_stream_parm_buffer_t;
typedef void cam_bundle_config_t;

typedef struct{
    uint8_t data[8*4];
} cam_mp_len_offset_t;

typedef struct {
    int32_t width;
    int32_t height;
} cam_dimension_t;

typedef struct {
    uint32_t num_planes;
    cam_mp_len_offset_t mp[VIDEO_MAX_PLANES];
    uint32_t frame_len;
} cam_frame_len_offset_t;

typedef struct {
    cam_frame_len_offset_t plane_info;
} cam_stream_buf_plane_info_t;

typedef struct {
    /* stream ID from server */
    uint32_t stream_svr_id;

    /* stream type */
    cam_stream_type_t stream_type;

    /* image format */
    cam_format_t fmt;

    /* image dimension */
    cam_dimension_t dim;

    /* buffer plane information, will be calc based on stream_type, fmt,
    dim, and padding_info(from stream config). Info including:
    offset_x, offset_y, stride, scanline, plane offset */
    cam_stream_buf_plane_info_t buf_planes;

    /* number of stream bufs will be allocated */
    uint32_t num_bufs;

    /* streaming type */
    cam_streaming_mode_t streaming_mode;

    /* num of frames needs to be generated.
     * only valid when streaming_mode = CAM_STREAMING_MODE_BURST */
    uint8_t num_of_burst;

    /* Other data */
    uint8_t data[159*4];
} cam_stream_info_t;

typedef struct {
    uint8_t denoise_enable;
    cam_denoise_process_type_t process_plates;
} cam_denoise_param_t;

typedef struct {
    uint32_t width_padding;
    uint32_t height_padding;
    uint32_t plane_padding;
} cam_padding_info_t;

typedef struct  {
    int32_t left;
    int32_t top;
    int32_t width;
    int32_t height;
} cam_rect_t;

#ifdef __cplusplus
}
#endif


#endif