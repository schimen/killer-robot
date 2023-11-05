#include "motor.h"

// Use logger from main.c
LOG_MODULE_DECLARE(brain);

// How much the speed will chenge each interval
#define DC_MOTOR_SPEED_STEP 10
#define BL_MOTOR_SPEED_STEP 2

// Length of delay between each change of speed
#define DC_MOTOR_SPEED_INTERVAL K_MSEC(40)
#define BL_MOTOR_SPEED_INTERVAL K_MSEC(20)

// Create threads for gradually controlling motor speed
#define STACK_SIZE 512
#define THREAD_PRIORITY 5
#define NUM_THREADS 2
// Structs for motor control threads
static struct k_thread motor_threads[NUM_THREADS];
// Define stack area for motor threads
K_THREAD_STACK_ARRAY_DEFINE(stacks, NUM_THREADS, STACK_SIZE);

// Define brushless and dc motor pwm periods
static const uint32_t DC_MOTOR_PERIOD = PWM_MSEC(1U);
static const uint32_t BL_MOTOR_PERIOD = PWM_MSEC(20U);
static const uint32_t BL_MOTOR_PULSE_MID = PWM_USEC(1500);
static const uint32_t BL_MOTOR_PULSE_RANGE = PWM_USEC(500U);
static const uint32_t BL_MOTOR_PULSE_MIN =
    BL_MOTOR_PULSE_MID - BL_MOTOR_PULSE_RANGE;
static const uint32_t BL_MOTOR_PULSE_MAX =
    BL_MOTOR_PULSE_MID + BL_MOTOR_PULSE_RANGE;

void dc_motor_init(struct motor_control *motor, int motor_index) {
    if (motor_index > NUM_THREADS - 1) {
        LOG_ERR(
            "Could not initialize motor with index %d. Can only have %d motors",
            motor_index, NUM_THREADS);
        return;
    }
    motor->speed = 127;
    motor->set_speed_func = &set_dc_motor_speed;
    motor->speed_step = DC_MOTOR_SPEED_STEP;
    motor->interval = DC_MOTOR_SPEED_INTERVAL;
    motor->thread =
        k_thread_create(&motor_threads[motor_index], stacks[motor_index],
                        STACK_SIZE, motor_speed_thread_callback, motor, NULL,
                        NULL, THREAD_PRIORITY, 0, K_NO_WAIT);
    set_dc_motor_speed(&motor->en1, &motor->en2, motor->speed);
}

void bl_motor_init(struct motor_control *motor, int motor_index) {
    if (motor_index > NUM_THREADS - 1) {
        LOG_ERR(
            "Could not initialize motor with index %d. Can only have %d motors",
            motor_index, NUM_THREADS);
        return;
    }
    motor->speed = 127;
    motor->set_speed_func = &set_bl_motor_speed;
    motor->speed_step = BL_MOTOR_SPEED_STEP;
    motor->interval = BL_MOTOR_SPEED_INTERVAL;
    motor->thread =
        k_thread_create(&motor_threads[motor_index], stacks[motor_index],
                        STACK_SIZE, motor_speed_thread_callback, motor, NULL,
                        NULL, THREAD_PRIORITY, 0, K_NO_WAIT);
    set_bl_motor_speed(&motor->en1, NULL, motor->speed);
}

void dc_motor_off(struct motor_control *motor) {
    pwm_set_dt(&motor->en1, DC_MOTOR_PERIOD, 0);
    pwm_set_dt(&motor->en2, DC_MOTOR_PERIOD, 0);
    motor->speed = 127;
}

void bl_motor_off(struct motor_control *motor) {
    pwm_set_dt(&motor->en1, BL_MOTOR_PERIOD, BL_MOTOR_PULSE_MID);
    motor->speed = 127;
}

void set_speed(struct motor_control *motor, uint8_t speed_u) {
    motor->speed = speed_u;
    k_thread_resume(motor->thread);
}

int32_t convert_speed(uint8_t speed_u) {
    int32_t speed = speed_u - 127;
    if (speed > 100) {
        speed = 100;
    } else if (speed < -100) {
        speed = -100;
    }
    return speed;
}

int set_dc_motor_speed(const struct pwm_dt_spec *en1,
                       const struct pwm_dt_spec *en2, uint8_t speed_u) {
    int32_t speed = convert_speed(speed_u);
    uint32_t pulse;
    int err1, err2;
    if (speed == 0) {
        err1 = pwm_set_dt(en1, DC_MOTOR_PERIOD, 0);
        err2 = pwm_set_dt(en2, DC_MOTOR_PERIOD, 0);
    } else if (speed > 0) {
        pulse = speed * ((DC_MOTOR_PERIOD - 1) / 100);
        if (pulse > DC_MOTOR_PERIOD) {
            pulse = DC_MOTOR_PERIOD;
        }
        err1 = pwm_set_dt(en1, DC_MOTOR_PERIOD, pulse);
        err2 = pwm_set_dt(en2, DC_MOTOR_PERIOD, 0);
    } else {
        speed = -1 * speed;
        pulse = speed * ((DC_MOTOR_PERIOD - 1) / 100);
        if (pulse > DC_MOTOR_PERIOD) {
            pulse = DC_MOTOR_PERIOD;
        }
        err1 = pwm_set_dt(en1, DC_MOTOR_PERIOD, 0);
        err2 = pwm_set_dt(en2, DC_MOTOR_PERIOD, pulse);
    }
    return err1 | err2;
}

int set_bl_motor_speed(const struct pwm_dt_spec *en1,
                       const struct pwm_dt_spec *en2, uint8_t speed_u) {
    ARG_UNUSED(en2);
    int32_t speed = convert_speed(speed_u);
    uint32_t pulse_offset = speed * (BL_MOTOR_PULSE_RANGE / 100);
    uint32_t pulse = BL_MOTOR_PULSE_MID + pulse_offset;
    if (pulse > BL_MOTOR_PULSE_MAX) {
        pulse = BL_MOTOR_PULSE_MAX;
    } else if (pulse < BL_MOTOR_PULSE_MIN) {
        pulse = BL_MOTOR_PULSE_MIN;
    }
    return pwm_set_dt(en1, BL_MOTOR_PERIOD, pulse);
}

void motor_speed_thread_callback(void *p1, void *p2, void *p3) {
    // Motor control struct
    struct motor_control *motor = p1;
    // Thread id of this thread
    k_tid_t thread_id = motor->thread;
    // Speed-step of this motor
    uint8_t step = motor->speed_step;
    // Timoeut of this function
    k_timeout_t interval = motor->interval;
    // Variable to keep desired speed, current speed and the difference
    uint8_t speed_u;
    uint8_t current_speed_u = 127;
    int16_t difference;
    // Callback function for setting the speed
    int (*set_speed_func)(const struct pwm_dt_spec *,
                          const struct pwm_dt_spec *, uint8_t) =
        motor->set_speed_func;

    while (true) {
        // Suspend before run
        if (motor->speed == current_speed_u) {
            k_thread_suspend(thread_id);
        }

        // Motor speed (unsigned)
        speed_u = motor->speed;

        if (speed_u < 27) {
            speed_u = 27;
        } else if (speed_u > 227) {
            speed_u = 227;
        }
        while (speed_u != current_speed_u) {
            speed_u = motor->speed;
            difference = speed_u - current_speed_u;
            if (difference > step) {
                difference = step;
            } else if (difference < -step) {
                difference = -step;
            }
            current_speed_u += difference;
            (*set_speed_func)(&motor->en1, &motor->en2, current_speed_u);
            k_sleep(interval);
        }
    }
}
