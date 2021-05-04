#include "utils.h"
#include "spine.h"
#include <signal.h>

int64_t last_time_of_write_frames = 0;
uint32_t leds[] = {0x0, 0x0, 0x0, 0x0};
int16_t motors[] = {0, 0, 0, 0};
uint32_t write_frames_seq = 0;
int spine_handle = 0;

spine_dataframe_t spine_data;
uint8_t spine_ver[40];
bool spine_ver_recieved = false;

void debug_dataframe(spine_dataframe_t *data)
{
    printf("seq: %d\n", data->seq);
    printf("status: 0x%02x\n", data->status);
    printf("i2c faults: %02x %02x\n", data->i2c_device_fault, data->i2c_fault_item);
    printf("encoders: WL: (pos %08d, dlt %08d, tm %010u), WR: (pos %08d, dlt %08d, tm %010u)\n", data->motors[0].pos,data->motors[0].dlt,data->motors[0].tm,  data->motors[1].pos,data->motors[1].dlt,data->motors[1].tm);
    printf("encoders: L: (pos %08d, dlt %08d, tm %010u), H: (pos %08d, dlt %08d, tm %010u)\n", data->motors[2].pos,data->motors[2].dlt,data->motors[2].tm,  data->motors[3].pos,data->motors[3].dlt,data->motors[3].tm);
    printf("cliff: %4u %4u %4u %4u\n", data->cliff_sensor[0], data->cliff_sensor[1], data->cliff_sensor[2], data->cliff_sensor[3]);
    printf("bat_voltage:%02.2f chrg_voltage:%02.2f body_temp:%04d bat_flags:%04x\n", data->battery_voltage*0.00136719, data->charger_voltage*0.00136719, data->body_temp, data->battery_flags);
    printf("prox: range %u, rate %u, sig %u, amb %u, spadCnt %u, sampCnt %u, calibRes %u\n", data->prox_raw_range_mm, data->prox_signal_rate_mcps, data->prox_sigma_mm, data->prox_ambient, data->prox_SPAD_count, data->prox_sample_count, data->prox_calibration_result);
    printf("button: %06d touch_sensor: %06d\n", data->buttton_state, data->touch_sensor);
    printf("button inputs: %06d\n", data->button_inputs);
    printf("mic_indices: %010u\n", data->mic_indices);
    printf("mic: \n");
    debug_buffer((uint8_t*)data->mic_data, sizeof(data->mic_data));

    printf("__reserved1: %x\n", data->__reserved1);
    printf("__reserved2:\n");
    debug_buffer(data->__reserved2, sizeof(data->__reserved2));
}

void spine_write_frames(int handle)
{
    int64_t now = get_time();
    if ((now-last_time_of_write_frames) > 5000)
    {
        if (!spine_ver_recieved)
        {
            spine_request_version(handle);
        }
        spine_update(handle, write_frames_seq++, 0, motors, leds);
    }
}

void spine_read_frames(int handle)
{
    int rc = 0;
    while (1)
    {
        do {
            rc = spine_parse_frame();
        }
        while (rc < 0);
        if (!rc) break;
        if (parsed_frame_size)
        {
            if (parsed_frame_type == 0x6466) {
                memcpy(&spine_data, parsed_frame_data, parsed_frame_size);
            }
            else if (parsed_frame_type == 0x7276) {
                memcpy(spine_ver, parsed_frame_data, parsed_frame_size);
                spine_ver_recieved = true;
            }
            else {
                printf("%s: ERROR: unknwon type: %x", __func__, parsed_frame_type);
                exit(-1);
            }
            parsed_frame_size = 0;
        }   
        break;
    }
    spine_read(handle);
}

int spine_main_loop()
{
    printf("\033[%d;%dH\n", 0, 0);
    debug_dataframe(&spine_data);
    printf("ver: ");
    debug_buffer(spine_ver, sizeof(spine_ver));
    return 0;
}

void spine_led_demo(int handle) {
    uint8_t led_data[] = {0xFF,0x00,0x00, 0x00,0xFF,0x00, 0x00,0x00,0xFF, 0xFF,0x00,0x00, 0x00,0x00,0x00,0x00};
    spine_set_led(handle, (uint32_t*)led_data);
    delay(500);
    memset(led_data, 0, 16);
    spine_set_led(handle, (uint32_t*)led_data);
}

void spine_motors_leds_demo(int handle) {
    uint32_t leds[] = {0x0, 0x0, 0x0, 0x0};
    int16_t motors[] = {0, 0, -10000, -10000};
    int i;
    for(i=0;i<500; i++)
    {
        spine_update(handle, 8888+i, 0, motors, leds);
        delay(1);
    }
    delay(500);
    for(i=0;i<100; i++)
    {
        motors[0] = 10000+i*100;
        motors[1] = motors[0];
        motors[2] = 6000+i*100;
        motors[3] = 6000+i*100;
        leds[0] = 50+i*2;
        leds[1] = (50+i*2)<<8;
        leds[2] = (50+i*2)<<16;
        spine_update(handle, 8888+i, 0, motors, leds);
        delay(1);
    }
    delay(500);
    for(i=0;i<100; i++)
    {
        motors[0] = -(10000+i*100);
        motors[1] = motors[0];
        
        leds[0] = 250-i*2;
        leds[1] = (250-i*2)<<8;
        leds[2] = (250-i*2)<<16;
        motors[2] = -(6000+i*100);
        motors[3] = -(6000+i*100);
        spine_update(handle, 8888+i, 0, motors, leds);
        delay(1);
    }
}

void spine_demo_exit(int signum)
{
    printf("Exiting on %d signal\n", signum);
    if (spine_handle > 0)
        close(spine_handle);
    exit(signum); 
}

void spine_demo() {
    signal(SIGINT, spine_demo_exit);
    spine_handle = spine_init();
    if (spine_handle > 0) {
        // spine_mode_change(spine_handle);
        // spine_motors_leds_demo(spine_handle);
        spine_led_demo(spine_handle);
        delay(500);
        // spine_shutdown(spine_handle);

        memset(&spine_data, 0, sizeof(spine_data));
        memset(spine_ver, 0, sizeof(spine_ver));
        parsed_frame_size = 0;
        parsed_frame_type = 0;
        while(!spine_main_loop())
        {
            spine_write_frames(spine_handle);
            spine_read_frames(spine_handle);
        }


        close(spine_handle);
    }
}