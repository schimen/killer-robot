#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "gatt_command.h"
#include "motor.h"

const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

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
    if (gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE)) {
        printk("No led0 configured");
    } else {
        blink_wt(&led0);
    }

    // Start bluetooth service
    if (peripheral_init()) {
        printk("Could not init ble peripheral\n");
        return;
    }
    printk("Bluetooth service started, waiting for commands\n");

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