
#ifndef __VIDEODEV2_H__
#define __VIDEODEV2_H__

// System dependencies
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MSM_CAMERA_MAX_USER_BUFF_CNT 16
#define MSM_CAMERA_PRIV_CMD_MAX  20

#define VIDEO_MAX_PLANES               4

#define V4L2_CID_PRIVATE_BASE		0x08000000

/**
 * struct v4l2_plane - plane info for multi-planar buffers
 * @bytesused:		number of bytes occupied by data in the plane (payload)
 * @length:		size of this plane (NOT the payload) in bytes
 * @mem_offset:		when memory in the associated struct v4l2_buffer is
 *			V4L2_MEMORY_MMAP, equals the offset from the start of
 *			the device memory for this plane (or is a "cookie" that
 *			should be passed to mmap() called on the video node)
 * @userptr:		when memory is V4L2_MEMORY_USERPTR, a userspace pointer
 *			pointing to this plane
 * @fd:			when memory is V4L2_MEMORY_DMABUF, a userspace file
 *			descriptor associated with this plane
 * @data_offset:	offset in the plane to the start of data; usually 0,
 *			unless there is a header in front of the data
 *
 * Multi-planar buffers consist of one or more planes, e.g. an YCbCr buffer
 * with two planes can have one plane for Y, and another for interleaved CbCr
 * components. Each plane can reside in a separate memory buffer, or even in
 * a completely separate memory node (e.g. in embedded devices).
 */
struct v4l2_plane {
	uint32_t			bytesused;
	uint32_t			length;
	union {
		uint32_t		mem_offset;
		unsigned long	userptr;
		uint32_t		fd;
	} m;
	uint32_t			data_offset;
	uint32_t			reserved[11];
};

#endif