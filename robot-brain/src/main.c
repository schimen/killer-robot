#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#include "gatt_command.h"
#include "motor.h"

const struct pwm_dt_spec a1 = PWM_DT_SPEC_GET(DT_ALIAS(a1));
const struct pwm_dt_spec a2 = PWM_DT_SPEC_GET(DT_ALIAS(a2));
const struct pwm_dt_spec b1 = PWM_DT_SPEC_GET(DT_ALIAS(b1));
const struct pwm_dt_spec b2 = PWM_DT_SPEC_GET(DT_ALIAS(b2));
const struct pwm_dt_spec weapon = PWM_DT_SPEC_GET(DT_ALIAS(weapon));

void main(void) {
    // Start bluetooth service
    if (peripheral_init()) {
        printk("Could not init ble peripheral\n");
        return;
    }
    printk("Bluetooth service started, waiting for commands\n");

    // Receive incoming commands
    struct command_data command;
    while (get_command(&command) == 0) {
        printk("Key: %d, id: %d, value: %d\n", command.key, command.id, command.value);
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
                set_motor_speed(&a1, &a2, command.value);
                send_ack(command, 0);
                break;

            case right_motor_command:
                set_motor_speed(&b1, &b2, command.value);
                send_ack(command, 0);
                break;
            
            case weapon_motor_command:
                set_weapon_speed(&weapon, command.value);
                send_ack(command, 0);
                break;

            default: // unrecognized command, send error
                send_error(command, error_unrecognized);
                break;
        }
    }
}