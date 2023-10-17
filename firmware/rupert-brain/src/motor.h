#ifndef MOTOR_H
#define MOTOR_H

#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/** @brief Structure for motor control. */
struct motor_control {
    /* ID of thread that controls motor */
    k_tid_t thread;
    /* Desired speed of motor, represented in unsigned format */
    uint8_t speed;
    /* Callback function for setting the speed of the motor */
    int (*set_speed_func)(const struct pwm_dt_spec *,
                          const struct pwm_dt_spec *, uint8_t);
    /* First enable pin of motor */
    const struct pwm_dt_spec en1;
    /* Second enable pin of motor */
    const struct pwm_dt_spec en2;
    /* Speed step for motor */
    uint8_t speed_step;
    /* Motor speed-set interval */
    k_timeout_t interval;
};

/**
 * @brief Initialize threads and structs for dc motor
 *
 * @param motor motor_control struct
 * @param motor_index index of motor, start with 0 and add 1 for each motor
 * initialized
 */
void dc_motor_init(struct motor_control *motor, int motor_index);

/**
 * @brief Initialize threads and structs for brushless motor
 *
 * @param motor motor_control struct
 * @param motor_index index of motor, start with 0 and add 1 for each motor
 * initialized
 */
void bl_motor_init(struct motor_control *motor, int motor_index);

/**
 * @brief Set 0 duty cycle to dc motor
 *
 * @param motor motor_control struct of relevant motor
 */
void dc_motor_off(struct motor_control *motor);

/**
 * @brief Set 0 speed to brushless motor
 *
 * @param motor motor_control struct of relevant motor
 */
void bl_motor_off(struct motor_control *motor);

/**
 * @brief Set speed of a motor
 *
 * @param motor motor_control struct that decides how motor speed will be set.
 * @param speed_u Speed of motor represented by unsigned integer.
 */
void set_speed(struct motor_control *motor, uint8_t speed_u);

/**
 * @brief Set speed of DC motor via motor driver.
 *   (This function is intended to be used as a callback function
 *    in motor_control struct)
 *
 * @param en1 Enable pin 1 for DC motor
 * @param en2 Enable pin 2 for DC motor
 * @param speed_u Speed represented in unsigned format
 * @return int Return status according to `pwm_set_dt` function.
 */
int set_dc_motor_speed(const struct pwm_dt_spec *en1,
                       const struct pwm_dt_spec *en2, uint8_t speed_u);

/**
 * @brief Set speed of brushless motor via ESC.
 *   (This function is intended to be used as a callback function
 *    in motor_control struct)
 *
 * @param en1 PWM pin
 * @param en2 Unused
 * @param speed_u Speed represented in unsigned format
 * @return int Return status according to `pwm_set_dt` function.
 */
int set_bl_motor_speed(const struct pwm_dt_spec *weapon_pin,
                       const struct pwm_dt_spec *unused, uint8_t speed_u);

/**
 * @brief Callback for motor control threads.
 *   This function is run in a thread for each motor to control their speed
 *   gradually.
 *
 * @param p1 motor_controller struct
 * @param p2 Unused
 * @param p3 Unused
 */
void motor_speed_thread_callback(void *p1, void *p2, void *p3);

#endif