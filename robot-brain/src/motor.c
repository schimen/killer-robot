#include "motor.h"

static const uint32_t MOTOR_PERIOD = PWM_MSEC(1U);
static const uint32_t WEAPON_PERIOD = PWM_MSEC(1U);
static const uint32_t WEAPON_PULSE_MID = WEAPON_PERIOD/2;
static const uint32_t WEAPON_PULSE_RANGE = WEAPON_PERIOD/4;


void set_motor_speed(const struct pwm_dt_spec *en1, const struct pwm_dt_spec *en2, int8_t speed) {
    float speed_fraction = abs(speed)/100;
    if (speed_fraction > 1) {
        speed_fraction = 1;
    }
    uint32_t pulse = MOTOR_PERIOD*speed_fraction;
    if (speed > 0) {
        printk("Set motor1 with %d duty\n", abs(speed));
        pwm_set_dt(en1, MOTOR_PERIOD, pulse);
        pwm_set_dt(en2, MOTOR_PERIOD, 0);
    }
    else {
        printk("Set motor2 with %d duty\n", abs(speed));
        pwm_set_dt(en1, MOTOR_PERIOD, 0);
        pwm_set_dt(en2, MOTOR_PERIOD, pulse);
    }
}

void set_weapon_speed(const struct pwm_dt_spec *weapon_pin, int8_t speed) {
    float speed_fraction = abs(speed)/100;
    if (speed_fraction > 1) {
        speed_fraction = 1;
    }
    if (speed < 0) {
        speed_fraction *= -1;
    }
    printk("Set weapon with duty %d\n", speed);
    uint32_t pulse = WEAPON_PULSE_MID + WEAPON_PULSE_RANGE*speed_fraction;
    if (pwm_set_dt(weapon_pin, WEAPON_PERIOD, pulse)) {
        printk("Pwm not working :(");
    }
}