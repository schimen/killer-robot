#include "motor.h"

static const uint32_t MOTOR_PERIOD = PWM_MSEC(20U);
static const uint32_t WEAPON_PERIOD = PWM_MSEC(20U);
#define WEAPON_PULSE_MIN PWM_MSEC(1U)
#define WEAPON_PULSE_MAX PWM_MSEC(2U)
static const uint32_t WEAPON_PULSE_RANGE = WEAPON_PULSE_MAX - WEAPON_PULSE_MIN;
static const uint32_t WEAPON_PULSE_MID = WEAPON_PULSE_MIN + WEAPON_PULSE_RANGE/2;

int32_t convert_speed(uint8_t speed_u) {
    int32_t speed = speed_u - 127;
    if (speed > 100) {
        speed = 100;
    }
    else if (speed < -100) {
        speed = -100;
    }
    return speed;
}

int set_motor_speed(const struct pwm_dt_spec *en1, const struct pwm_dt_spec *en2, uint8_t speed_u) {
    int32_t speed = convert_speed(speed_u);
    uint32_t pulse;
    int err1, err2;
    if (speed == 0) {
        err1 = pwm_set_dt(en1, MOTOR_PERIOD, 0);
        err2 = pwm_set_dt(en2, MOTOR_PERIOD, 0);
    }
    else if (speed > 0) {
        pulse = speed*(MOTOR_PERIOD/100);
        err1 = pwm_set_dt(en1, MOTOR_PERIOD, pulse);
        err2 = pwm_set_dt(en2, MOTOR_PERIOD, 0);
    }
    else {
        speed = -1*speed;
        pulse = speed*(MOTOR_PERIOD/100);
        err1 = pwm_set_dt(en1, MOTOR_PERIOD, 0);
        err2 = pwm_set_dt(en2, MOTOR_PERIOD, pulse);
    }
    return err1 | err2;
}

int set_weapon_speed(const struct pwm_dt_spec *weapon_pin, uint8_t speed_u) {
    int32_t speed = convert_speed(speed_u);
    uint32_t pulse_offset = speed*(WEAPON_PULSE_RANGE/100);
    uint32_t pulse = WEAPON_PULSE_MID + pulse_offset;
    return pwm_set_dt(weapon_pin, WEAPON_PERIOD, pulse);
}