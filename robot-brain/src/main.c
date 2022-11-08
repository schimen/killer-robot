#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

#include "gatt_command.h"
#include "uart_command.h"
#include "motor.h"

// Register logger
LOG_MODULE_REGISTER(brain);

// GPIO structs
const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec nsleep = GPIO_DT_SPEC_GET(DT_ALIAS(nsleep), gpios);
struct gpio_callback sleep_callback;

// Serial interface (HC12)
struct serial_interface hc12_iface;
struct command_writer hc12_writer;
const struct gpio_dt_spec hc12_set = GPIO_DT_SPEC_GET(DT_ALIAS(hc12_set), gpios);
const struct device *hc12_device = DEVICE_DT_GET(DT_ALIAS(hc12));

struct motor_control motor_a = {
    .en1 = PWM_DT_SPEC_GET(DT_ALIAS(a1)),
    .en2 = PWM_DT_SPEC_GET(DT_ALIAS(a2)),
};

struct motor_control motor_b = {
    .en1 = PWM_DT_SPEC_GET(DT_ALIAS(b1)),
    .en2 = PWM_DT_SPEC_GET(DT_ALIAS(b2)),
};

struct motor_control weapon = {
    .en1 = PWM_DT_SPEC_GET(DT_ALIAS(weapon)),
};

// Create workthread
#define WORKTHREAD_SIZE 512
#define WORKTHREAD_PRIO 5
K_THREAD_STACK_DEFINE(workthread_area, WORKTHREAD_SIZE);
struct k_work_q work_q;

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
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    // Receive incoming commands
    struct command_data command;
    while (get_command(&command) == 0) {
        LOG_INF(
            "New command %d (id %d) with value %d", 
            command.key, command.id, command.value
        );
        blink_wt();
        switch (command.key) {
        case error_command: // an error ocurred!
            // acknowledge command, even though an error occurred
            set_ack(command.id);
            LOG_WRN(
                "Warning: received error command (error %d)", 
                command.value
            );
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

        case weapon_motor_command:
            set_speed(&weapon, command.value);
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
K_THREAD_DEFINE(msg_handler_tid, MSGTHREAD_SIZE,
                message_handler, NULL, NULL, NULL,
                MSGTHREAD_PRI, 0, 0);

static void start_sleep() {
    // Stop communication
    k_thread_suspend(msg_handler_tid);
    peripheral_disable();
    serial_disable(&hc12_iface);
    // Turn off motors and weapon
    motor_off(&motor_a);
    motor_off(&motor_b);
    weapon_off(&weapon);

    LOG_INF("Sleep mode on");
    blink();
}

static void stop_sleep() {
    serial_enable(&hc12_iface);
    peripheral_enable();
    k_thread_resume(msg_handler_tid);
    LOG_INF("Sleep mode off");
    blink();
}

// Define sleep and wake workers 
// (sleep and wake device in workthread, not interrupt)
K_WORK_DEFINE(sleep_worker, start_sleep);
K_WORK_DEFINE(wake_worker, stop_sleep);

/**
 * @brief Callback for sleep-pin interrupt on sleep
 */
static void sleep_handler(const struct device *port, struct gpio_callback *cb, uint32_t pins)
{
    ARG_UNUSED(port); ARG_UNUSED(cb); ARG_UNUSED(pins);
    static int sleep_mode = 0;
    // Read nsleep pin to see if device should sleep or wake up
    int state = gpio_pin_get_dt(&nsleep);
    if (state < 0) {
        LOG_ERR(
            "Error %d: failed to read %s pin %d", 
            state, nsleep.port->name, nsleep.pin
        );
    }
    // Save state so it does not duplicate
    if (sleep_mode != state) {
        sleep_mode = state;
    }
    if (sleep_mode) {
        k_work_submit(&sleep_worker);
    }
    else {
        k_work_submit(&wake_worker);
    }
}

void main(void) {
    // Save error codes during initialization
    int err;

    // Init and start workqueue
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, workthread_area, WORKTHREAD_SIZE,
                       WORKTHREAD_PRIO, NULL);

    // Init motor control
    motor_init(&motor_a, &motor_b, &weapon);

    // Initialize serial interface (HC12)
    serial_init(&hc12_iface, &hc12_writer, hc12_device, &hc12_set);

    // Start bluetooth service
    peripheral_enable();

    // Configure nsleep pin and add sleep callback to it
    err = gpio_pin_configure_dt(&nsleep, GPIO_INPUT);
    if (err) {
        LOG_ERR(
            "Error %d: failed to configure %s pin %d", 
            err, nsleep.port->name, nsleep.pin
        );
    } else {
        // Start sleep if pin high from start
        if (gpio_pin_get_dt(&nsleep)) {
            start_sleep();
        }
    }
    err = gpio_pin_interrupt_configure_dt(&nsleep, GPIO_INT_EDGE_BOTH);
    if (err) {
        LOG_ERR(
            "Error %d: failed to configure interrupt on %s pin %d",
            err, nsleep.port->name, nsleep.pin
        );
    }
    gpio_init_callback(&sleep_callback, sleep_handler, BIT(nsleep.pin));
    err = gpio_add_callback(nsleep.port, &sleep_callback);
    if (err) {
        LOG_ERR(
            "Error %d: failed to add callback to %s", 
            err, nsleep.port->name
        );
    }

    // Init led0
    err = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    if (err) {
        LOG_ERR(
            "Error %d: failed to configure %s pin %d",
            err, led0.port->name, led0.pin
        );
    }

    // Blink led when ready
    blink_wt(&led0);
    LOG_INF("Setup finished");
}
