#ifndef MOTOR_H
#define MOTOR_H

#include <stdlib.h>
#include <zephyr/drivers/pwm.h>

int set_motor_speed(const struct pwm_dt_spec *en1, const struct pwm_dt_spec *en2, uint8_t speed_u);

int set_weapon_speed(const struct pwm_dt_spec *weapon_pin, uint8_t speed_u);

#endif