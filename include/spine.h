#ifndef __SPINE_H
#define __SPINE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_SPINE 0

typedef struct {
    int32_t pos;
    int32_t dlt;
    uint32_t tm;
} spine_motor_status_t;

typedef struct {
    uint32_t seq;
    uint16_t status;
    uint8_t i2c_device_fault;
    uint8_t i2c_fault_item;
    spine_motor_status_t motors[4];
    uint16_t cliff_sensor[4];
    int16_t battery_voltage;
    int16_t charger_voltage;
    int16_t body_temp;
    uint16_t battery_flags;
    uint16_t __reserved1;
    uint8_t prox_sigma_mm;
    uint16_t prox_raw_range_mm;
    uint16_t prox_signal_rate_mcps;
    uint16_t prox_ambient;
    uint16_t prox_SPAD_count;
    uint16_t prox_sample_count;
    uint32_t prox_calibration_result;
    uint16_t touch_sensor;
    uint16_t buttton_state;
    uint32_t mic_indices;
    uint16_t button_inputs;
    uint8_t __reserved2[26];
    uint16_t mic_data[320];
} spine_dataframe_t;

int spine_init();
int inline spine_send_frame(int handle, uint16_t frame_type, void* frame_data, uint16_t frame_size);
int spine_request_version(int handle);
int spine_mode_change(int handle);
int spine_mode_change(int handle);
int spine_shutdown(int handle);
int spine_set_led(int handle, uint32_t* led_data);
int spine_update(int handle, uint32_t seq, uint32_t flags, int16_t* motors_data, uint32_t* leds_data);
int spine_read(int handle);
int spine_parse_frame();


extern uint16_t parsed_frame_type;
extern uint16_t parsed_frame_size;
extern uint8_t parsed_frame_data[768];


#ifdef __cplusplus
}
#endif


#endif