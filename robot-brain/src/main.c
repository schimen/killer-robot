#include <zephyr/kernel.h>

#include "gatt_command.h"


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
    }
}