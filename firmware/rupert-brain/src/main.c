#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

#include "motor.h"
#include <command_lib/gatt_command.h>

// Register logger
LOG_MODULE_REGISTER(brain);

// GPIO structs
const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec nsleep = GPIO_DT_SPEC_GET(DT_ALIAS(nsleep), gpios);
struct gpio_callback sleep_callback;

// Sensor
const struct device *motion_sensor = DEVICE_DT_GET(DT_ALIAS(motion_sensor));

struct motor_control motor_a = {
    .en1 = PWM_DT_SPEC_GET(DT_ALIAS(a1)),
    .en2 = PWM_DT_SPEC_GET(DT_ALIAS(a2)),
};

struct motor_control motor_b = {
    .en1 = PWM_DT_SPEC_GET(DT_ALIAS(b1)),
    .en2 = PWM_DT_SPEC_GET(DT_ALIAS(b2)),
};

// Create workthread
K_THREAD_STACK_DEFINE(main_workthread_area, 512);
static struct k_work_q work_q;

/**
 * @brief Blink led0
 */
void blink() {
    gpio_pin_set_dt(&led0, 1);
    k_msleep(50);
    gpio_pin_set_dt(&led0, 0);
}

// Define blink worker
K_WORK_DEFINE(blink_worker, blink);

/**
 * @brief Blink led0 in workthread
 */
static inline void blink_wt() { k_work_submit(&blink_worker); }

/**
 * @brief Thread function that reads new messages and acts upon them
 *   This function is supposed to be used in a thread
 */
static void message_handler(void *p1, void *p2, void *p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    // Receive incoming commands
    struct command_data command;
    while (true) {
        if (get_command(&command) != 0) {
            // Did not receive command before timeout, turn motors off
            dc_motor_off(&motor_a);
            dc_motor_off(&motor_b);
            LOG_WRN("Command timed out, turn motor and weapon off");
            continue;
        }
        LOG_INF("New command %d (id %d) with value %d", command.key, command.id,
                command.value);
        blink_wt();
        switch (command.key) {
        case error_command: // an error ocurred!
            // acknowledge command, even though an error occurred
            set_ack(command.id);
            LOG_WRN("Warning: received error command (error %d)",
                    command.value);
            break;

        case ack_command: // set ack event when receiving ack
            set_ack(command.id);
            break;

        case ping_command: // ack ping command
            send_ack(command, 0);
            break;

        case left_motor_command:
            set_speed(&motor_a, command.value);
            send_ack(command, 0);
            break;

        case right_motor_command:
            set_speed(&motor_b, command.value);
            send_ack(command, 0);
            break;

        default: // unrecognized command, send error
            send_error(command, error_unrecognized);
            LOG_WRN("Warning: Unrecognized command");
            break;
        }
    }
}

// Define thread for handling incoming messages
#define MSGTHREAD_SIZE 1408
#define MSGTHREAD_PRI 4
K_THREAD_DEFINE(msg_handler_tid, MSGTHREAD_SIZE, message_handler, NULL, NULL,
                NULL, MSGTHREAD_PRI, 0, 0);

static void sleep_worker_func() {
    static int sleep_mode = 0;
    // 50ms debounce protection!
    k_msleep(50);
    // Read nsleep pin to see if device should sleep or wake up
    int state = gpio_pin_get_dt(&nsleep);
    if (state < 0) {
        LOG_ERR("Error %d: failed to read %s pin %d", state, nsleep.port->name,
                nsleep.pin);
    }
    // Return if sleep mode is equal to state
    if (sleep_mode == state) {
        return;
    }
    // Save state so it does not duplicate
    sleep_mode = state;
    if (sleep_mode) {
        // Stop communication
        k_thread_suspend(msg_handler_tid);
        // Turn off motors and weapon
        dc_motor_off(&motor_a);
        dc_motor_off(&motor_b);
        LOG_INF("Sleep mode on");
    } else {
        k_thread_resume(msg_handler_tid);
        LOG_INF("Sleep mode off");
    }
    blink();
}

// Define sleep worker
// (sleep and wake device in workthread, not interrupt)
K_WORK_DEFINE(sleep_worker, sleep_worker_func);

/**
 * @brief Callback for sleep-pin interrupt on sleep
 */
static void sleep_handler(const struct device *port, struct gpio_callback *cb,
                          uint32_t pins) {
    ARG_UNUSED(port);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);
    k_work_submit(&sleep_worker);
}

void main(void) {
    // Save error codes during initialization
    int err;

    // Init and start workqueue
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, main_workthread_area,
                       K_THREAD_STACK_SIZEOF(main_workthread_area), 5, NULL);

    // Init motor control
    dc_motor_init(&motor_a, 0);
    dc_motor_init(&motor_b, 1);

    // Start bluetooth service
    peripheral_enable();

    // Configure nsleep pin and add sleep callback to it
    err = gpio_pin_configure_dt(&nsleep, GPIO_INPUT);
    if (err) {
        LOG_ERR("Error %d: failed to configure %s pin %d", err,
                nsleep.port->name, nsleep.pin);
    } else {
        // Check for sleep mode at initialization
        k_work_submit(&sleep_worker);
    }
    err = gpio_pin_interrupt_configure_dt(&nsleep, GPIO_INT_EDGE_BOTH);
    if (err) {
        LOG_ERR("Error %d: failed to configure interrupt on %s pin %d", err,
                nsleep.port->name, nsleep.pin);
    }
    gpio_init_callback(&sleep_callback, sleep_handler, BIT(nsleep.pin));
    err = gpio_add_callback(nsleep.port, &sleep_callback);
    if (err) {
        LOG_ERR("Error %d: failed to add callback to %s", err,
                nsleep.port->name);
    }

    // Init led0
    err = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    if (err) {
        LOG_ERR("Error %d: failed to configure %s pin %d", err, led0.port->name,
                led0.pin);
    }

    // Blink led when ready
    blink_wt();
    LOG_INF("Setup finished");
}
