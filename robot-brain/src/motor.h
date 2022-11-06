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
};

/**
 * @brief Initialize threads and structs for motor control
 *
 * @param motor_a motor_control struct for motor a
 * @param motor_b motor_control struct for motor b
 * @param motor_w motor_control struct for weapon motor
 */
void motor_init(struct motor_control *motor_a, struct motor_control *motor_b,
                struct motor_control *motor_w);

/**
 * @brief Set 0 duty cycle to motor
 * 
 * @param motor motor_control struct of relevant motor
 */
void motor_off(struct motor_control *motor);

/**
 * @brief Set 0 duty cycle to wepon
 * 
 * @param motor motor_control struct of relevant weapon
 */
void weapon_off(struct motor_control *weapon);

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
int set_motor_speed(const struct pwm_dt_spec *en1,
                    const struct pwm_dt_spec *en2, uint8_t speed_u);

/**
 * @brief Set speed of brushless weapon motor via ESC.
 *   (This function is intended to be used as a callback function
 *    in motor_control struct)
 *
 * @param en1 PWM pin
 * @param en2 Unused
 * @param speed_u Speed represented in unsigned format
 * @return int Return status according to `pwm_set_dt` function.
 */
int set_weapon_speed(const struct pwm_dt_spec *weapon_pin,
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