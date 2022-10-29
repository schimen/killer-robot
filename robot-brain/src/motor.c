#include "motor.h"

// How much the speed will chenge each interval
#define SPEED_STEP 10
// Length of delay between each change of speed
#define SPEED_INTERVAL K_MSEC(40)

// Create threads for gradually controlling motor speed
#define STACK_SIZE 512
#define THREAD_PRIORITY 5
#define NUM_THREADS 3
// Structs for motor control threads
static struct k_thread motor_threads[NUM_THREADS];
// Define stack area for motor threads
K_THREAD_STACK_ARRAY_DEFINE(stacks, NUM_THREADS, STACK_SIZE);

// Define weapon and motor pwm periods
static const uint32_t MOTOR_PERIOD = PWM_MSEC(20U);
static const uint32_t WEAPON_PERIOD = PWM_MSEC(20U);
#define WEAPON_PULSE_MIN PWM_MSEC(1U)
#define WEAPON_PULSE_MAX PWM_MSEC(2U)
static const uint32_t WEAPON_PULSE_RANGE = WEAPON_PULSE_MAX - WEAPON_PULSE_MIN;
static const uint32_t WEAPON_PULSE_MID =
    WEAPON_PULSE_MIN + WEAPON_PULSE_RANGE / 2;

void motor_init(struct motor_control *motor_a, struct motor_control *motor_b,
                struct motor_control *motor_w) {
    motor_a->speed = 127;
    motor_a->set_speed_func = &set_motor_speed;
    motor_a->thread = k_thread_create(
        &motor_threads[0], stacks[0], STACK_SIZE, motor_speed_thread_callback,
        motor_a, NULL, NULL, THREAD_PRIORITY, 0, K_NO_WAIT);

    motor_b->speed = 127;
    motor_b->set_speed_func = &set_motor_speed;
    motor_b->thread = k_thread_create(
        &motor_threads[1], stacks[1], STACK_SIZE, motor_speed_thread_callback,
        motor_b, NULL, NULL, THREAD_PRIORITY, 0, K_NO_WAIT);

    motor_w->speed = 127;
    motor_w->set_speed_func = &set_weapon_speed;
    motor_w->thread = k_thread_create(
        &motor_threads[2], stacks[2], STACK_SIZE, motor_speed_thread_callback,
        motor_w, NULL, NULL, THREAD_PRIORITY, 0, K_NO_WAIT);
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

int set_motor_speed(const struct pwm_dt_spec *en1,
                    const struct pwm_dt_spec *en2, uint8_t speed_u) {
    int32_t speed = convert_speed(speed_u);
    uint32_t pulse;
    int err1, err2;
    if (speed == 0) {
        err1 = pwm_set_dt(en1, MOTOR_PERIOD, 0);
        err2 = pwm_set_dt(en2, MOTOR_PERIOD, 0);
    } else if (speed > 0) {
        pulse = speed * ((MOTOR_PERIOD-1) / 100);
        if (pulse > MOTOR_PERIOD) {
            pulse = MOTOR_PERIOD;
        }
        err1 = pwm_set_dt(en1, MOTOR_PERIOD, pulse);
        err2 = pwm_set_dt(en2, MOTOR_PERIOD, 0);
    } else {
        speed = -1 * speed;
        pulse = speed * ((MOTOR_PERIOD-1) / 100);
        if (pulse > MOTOR_PERIOD) {
            pulse = MOTOR_PERIOD;
        }
        err1 = pwm_set_dt(en1, MOTOR_PERIOD, 0);
        err2 = pwm_set_dt(en2, MOTOR_PERIOD, pulse);
    }
    return err1 | err2;
}

int set_weapon_speed(const struct pwm_dt_spec *weapon_pin,
                     const struct pwm_dt_spec *unused, uint8_t speed_u) {
    ARG_UNUSED(unused);
    int32_t speed = convert_speed(speed_u);
    uint32_t pulse_offset = speed * (WEAPON_PULSE_RANGE / 100);
    uint32_t pulse = WEAPON_PULSE_MID + pulse_offset;
    if (pulse > WEAPON_PERIOD) {
        pulse = WEAPON_PERIOD;
    }
    return pwm_set_dt(weapon_pin, WEAPON_PERIOD, pulse);
}

void motor_speed_thread_callback(void *p1, void *p2, void *p3) {
    // Motor control struct
    struct motor_control *motor = p1;
    // Thread id of this thread
    k_tid_t thread_id = motor->thread;
    // Variable to keep desired speed, current speed and the difference
    uint8_t speed_u;
    uint8_t current_speed_u = 127;
    int16_t difference;
    // Pointer to current speed
    uint8_t *current_speed_ptr = (uint8_t *)p2;
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
            if (difference > SPEED_STEP) {
                difference = SPEED_STEP;
            } else if (difference < -SPEED_STEP) {
                difference = -SPEED_STEP;
            }
            current_speed_u += difference;
            (*set_speed_func)(&motor->en1, &motor->en2, current_speed_u);
            k_sleep(SPEED_INTERVAL);
        }
    }
}