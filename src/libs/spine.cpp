#include "spine.h"
#include "utils.h"
#include <termios.h>
#include <sys/ioctl.h>

typedef struct{
    uint32_t header;
    uint16_t frame_type;
    uint16_t frame_size;
    uint8_t data[1280];
} spine_frame_t;

typedef struct {
    uint32_t seq;
    uint32_t flags;
    int16_t motors[4];
    uint8_t leds[12];
    uint8_t __reserved[28];
} spine_ctrl_t;

static spine_frame_t tx_frame;
uint8_t spine_rx_buf[0x1000];
int frame_buf_len = 0;
uint8_t frame_buf[0x2000];
uint16_t parsed_frame_type;
uint16_t parsed_frame_size;
uint8_t parsed_frame_data[768];

int spine_init() {
    int handle = open("/dev/ttyHS0", O_RDWR);
    int speed = B3000000;

    memset(spine_rx_buf, 0, sizeof(spine_rx_buf));
    memset(frame_buf, 0x55, sizeof(frame_buf));
    memset(parsed_frame_data, 0, sizeof(parsed_frame_data));
    
    if (handle < 0) {
        printf("%s: failed to open serial interface\n", __func__);
        return -3;
    }
    else {
        struct termios termios_p;
        if ( tcgetattr(handle, &termios_p) )
        {
            printf("%s: tcgetattr() failed\n", __func__);
            return -2;
        }
        cfmakeraw(&termios_p);
        cfsetispeed(&termios_p, speed);
        cfsetospeed(&termios_p, speed);
        termios_p.c_cflag |= 0x70u;
        if ( !tcsetattr(handle, 0, &termios_p) )
        {
            // todo: if ( !tcflush(handle, 2) )
            {
                return handle;
            }
        }
    }
    return -1;
}

ssize_t spine_write(int handle, void *buf, size_t len) {
    size_t bytes_to_send;
    ssize_t i;
    ssize_t bytes_written;
    bytes_to_send = len;
    i = 0;
    if ( (int)len >= 1 )
    {
        #if DEBUG_SPINE
        printf("data: ");
        for(int i=0;i<len;i++)
        {
            printf("%02x ", ((uint8_t*)buf)[i]);
        }
        printf("\n");
        #endif
        while ( 1 )
        {
            bytes_written = write(handle, buf, len);
            if ( bytes_written < 1 )
                break;
            #if DEBUG_SPINE
            printf("%s: written: %d\n", __func__, bytes_to_send);
            #endif
            i += bytes_written;
            buf = (char *)buf + bytes_written;
            len = bytes_to_send - i;
            if ( (int)(bytes_to_send - i) <= 0 )
                return i;
        }
        if ( bytes_written )
            i = bytes_written;
    }
    return i;
}

int inline spine_send_frame(int handle, uint16_t frame_type, void* frame_data, uint16_t frame_size) {
    tx_frame.header = 0x423248AA;
    tx_frame.frame_type = frame_type;
    tx_frame.frame_size = frame_size;
    if (frame_data && frame_size > 0) {
        memmove(&tx_frame.data, frame_data, tx_frame.frame_size);
    }
    uint32_t* pCRC = (uint32_t*)((uint32_t)&tx_frame.data+tx_frame.frame_size);
    *pCRC = get_CRC32((uint8_t*)frame_data, frame_size);
    int rc = spine_write(handle, (void*)&tx_frame, tx_frame.frame_size + 12);
    // todo: tcdrain(handle);
    return rc;
}

int spine_request_version(int handle) {
    spine_send_frame(handle, 0x7276, 0, 0);
}

int spine_mode_change(int handle) {
    spine_send_frame(handle, 0x6D64, 0, 0);
}

int spine_shutdown(int handle) {
    spine_send_frame(handle, 0x6473, 0, 0);
}

int spine_set_led(int handle, uint32_t* led_data) {
    return spine_send_frame(handle, 0x736C, led_data, 16);
}

int spine_update(int handle, uint32_t seq, uint32_t flags, int16_t* motors_data, uint32_t* leds_data) {
    spine_ctrl_t ctrl;
    memset(&ctrl, 0, sizeof(spine_ctrl_t));
    ctrl.seq = seq;
    ctrl.flags = flags;

    if (motors_data) {
        memmove(ctrl.motors, motors_data, 8);
    }
    if (leds_data) {
        ctrl.leds[0] = (leds_data[0]) & 0xFF;
        ctrl.leds[1] = (leds_data[0]>>8) & 0xFF;
        ctrl.leds[2] = (leds_data[0]>>16) & 0xFF;
        ctrl.leds[3] = (leds_data[1]>>0) & 0xFF;
        ctrl.leds[4] = (leds_data[1]>>8) & 0xFF;
        ctrl.leds[5] = (leds_data[1]>>16) & 0xFF;
        ctrl.leds[6] = (leds_data[2]) & 0xFF;
        ctrl.leds[7] = (leds_data[2]>>8) & 0xFF;
        ctrl.leds[8] = (leds_data[2]>>16) & 0xFF;
        ctrl.leds[9] = (leds_data[3]) & 0xFF;
        ctrl.leds[10] = (leds_data[3]>>8) & 0xFF;
        ctrl.leds[11] = (leds_data[3]>>16) & 0xFF;
    }
    return spine_send_frame(handle, 0x6466, &ctrl, 64);
}

inline int spine_check_readable(int handle) {
    if (!handle){ 
        printf("%s: handle is null error\n", __func__);
        return 0;
    }
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(handle, &rfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int err = select(0x400, &rfds, 0, 0, &tv);
    if (err <= -1) {
        char* err_str = strerror(err);
        printf("%s: select error[%d]: %s\n", __func__, err, err_str);
        return 0;
    }
    else if (err == 0) { 
        printf("%s: select timeout error\n", __func__);
        return 0;
    }
    return 1;
}

int spine_read(int handle) {

    if (!spine_check_readable(handle))
        return -1;
    int rx_len = read(handle, spine_rx_buf, sizeof(spine_rx_buf));
    if (rx_len <= 0) 
        return rx_len;
    else
    {
        if ((frame_buf_len + rx_len) > sizeof(frame_buf))
        {
            // data overflow
            frame_buf_len = 0;
        }
        memcpy(frame_buf+frame_buf_len, spine_rx_buf, rx_len);
        frame_buf_len += rx_len;
        return rx_len;
    }
    return 0;
}

int get_expected_frame_size(int frame_type, bool flag = true)
{
    int size = 0;
    if ( frame_type <= 0x6B60 )
    {
        if ( frame_type > 0x6472 )
        {
            if ( frame_type == 0x6473 )
                return size;
            if ( frame_type != 0x6662 )
            {
                if ( frame_type == 0x6675 )
                    return 1028;
                return -1;
            }
            return 4;
        }
        if ( frame_type == 0x6364 )
            return 32;
        if ( frame_type != 0x6466 )
            return -1;
        size = 768;
        if ( !flag )
            size = 64;
    }
    else
    {
        if ( frame_type <= 0x736B )
        {
            if ( frame_type != 0x6B61 )
            {
                if ( frame_type == 0x6D64 )
                    return size;
                if ( frame_type == 0x7276 )
                    return 40;
                return -1;
            }
            return 4;
        }
        if ( frame_type == 0x736C )
            return 16;
        bool is_ota_type = frame_type == 0x7374;
        if ( frame_type != 0x7374 )
            is_ota_type = frame_type == 0x7878;
        if ( !is_ota_type )
            return -1;
    }
    return size;
}

int spine_parse_frame()
{
    int ret = 0;
    if (frame_buf_len > 0)
    {
        int frame_start_offset = 0;
        bool header_found = false;
        while(frame_start_offset < (frame_buf_len-4))
        {
            if (*((uint32_t*)&frame_buf[frame_start_offset]) ==  0x483242aa)
            {
                header_found = true;
                break;
            }
            frame_start_offset++;
        }
        // printf("%s: frame found: %d, size: %d\n", __func__, header_found, frame_buf_len);
        // debug_buffer(frame_buf, 32);
        if (header_found)
        {
            memmove(frame_buf, &frame_buf[frame_start_offset], frame_buf_len-frame_start_offset);
            frame_buf_len = frame_buf_len-frame_start_offset;
            if ((sizeof(frame_buf) - frame_buf_len) >= 1)
                memset(frame_buf+frame_buf_len, 0x55, sizeof(frame_buf) - frame_buf_len);

            if (frame_buf_len >= 7)
            {
                uint16_t frame_type = *((uint16_t*)&frame_buf[4]);
                uint16_t frame_size = *((uint16_t*)&frame_buf[6]);
                uint16_t expected_frame_size = get_expected_frame_size(frame_type, true);
                    
                // debug_buffer(frame_buf, 8);
                // printf("%s: frame[%x] size: %d, expected size: %d\n", __func__, frame_type, frame_size, expected_frame_size);

                if (frame_size == expected_frame_size && frame_size <= frame_buf_len)
                {
                    uint32_t expected_crc32 = get_CRC32((uint8_t*)&frame_buf[8], frame_size);
                    uint32_t actual_crc32 = *((uint32_t*)(&frame_buf[8]+frame_size));
                    if (expected_crc32 == actual_crc32)
                    {
                        parsed_frame_type = frame_type;
                        parsed_frame_size = frame_size;
                        memcpy(parsed_frame_data, frame_buf+8, frame_size);

                        memmove(frame_buf, frame_buf+frame_size+12, frame_buf_len-(frame_size+12));
                        frame_buf_len = frame_buf_len-(frame_size+12);
                        // printf("%s: frame[%x] size: %d\n", __func__, parsed_frame_type, parsed_frame_size);
                    }
                }
            }
        }
        ret = frame_buf_len;
    }
    return ret;
}