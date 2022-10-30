#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "gatt_command.h"
#include "uart_command.h"
#include "motor.h"

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
    gpio_pin_set(led0.port, led0.pin, 1);
    k_msleep(50);
    gpio_pin_set(led0.port, led0.pin, 0);
}

/**
 * @brief Callback for sleep-pin interrupt on sleep
 */
static void sleep_handler(const struct device *port, struct gpio_callback *cb, uint32_t pins)
{
    static bool currently_sleeping = false;
    ARG_UNUSED(port); ARG_UNUSED(cb); ARG_UNUSED(pins);
    if (currently_sleeping) {
        printk("Wake interrupt");
        currently_sleeping = false;
    }
    else {
        printk("Sleep interrupt");
        currently_sleeping = true;
    }
}

// Define blink worker
K_WORK_DEFINE(blink_worker, blink);

/**
 * @brief Blink led0 in workthread
 *
 */
void blink_wt() { k_work_submit(&blink_worker); }

void main(void) {
    // Init and start workqueue
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, workthread_area, WORKTHREAD_SIZE,
                       WORKTHREAD_PRIO, NULL);

    // Init motor control
    motor_init(&motor_a, &motor_b, &weapon);

    // Init led0
    if (gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE) == 0) {
        blink_wt(&led0);
    }

    // Init sleep and wake interrupt
    if (gpio_pin_configure_dt(&nsleep, GPIO_INPUT) == 0) {
        gpio_init_callback(&sleep_callback, sleep_handler, BIT(nsleep.pin));
        if (gpio_add_callback(nsleep.port, &sleep_callback) == 0) {
            gpio_pin_interrupt_configure_dt(&nsleep, GPIO_INT_EDGE_BOTH);
        }
    }

    // Initialize serial interface (HC12)
    serial_init(&hc12_iface, &hc12_writer, hc12_device, &hc12_set);

    // Start bluetooth service
    peripheral_init();

    // Receive incoming commands
    struct command_data command;
    while (get_command(&command) == 0) {
        blink_wt();
        switch (command.key) {
        case error_command: // an error ocurred!
            // acknowledge command, even though an error occurred
            set_ack(command.id);
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
            break;
        }
    }
}