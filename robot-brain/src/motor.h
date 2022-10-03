#ifndef MOTOR_H
#define MOTOR_H

#include <stdlib.h>
#include <zephyr/drivers/pwm.h>

void set_motor_speed(const struct pwm_dt_spec *en1, const struct pwm_dt_spec *en2, int8_t speed);

void set_weapon_speed(const struct pwm_dt_spec *weapon_pin, int8_t speed);

#endif