//#include <device.h>
//#include <devicetree.h>
//#include <drivers/gpio.h>
#include <kernel.h>
//#include <sys/arch_interface.h>
#include <zephyr.h>

#include "gatt_command.h"
//#include "uart_command.h"

// leds
//const struct gpio_dt_spec user_led  = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

/**
 * @brief Initialize leds
 */
void led_init() {
    //gpio_pin_configure_dt(&user_led, GPIO_OUTPUT_INACTIVE);
}

/**
 * @brief Blink led
 */
void blink() {
    //gpio_pin_set(user_led.port, user_led.pin, 1);
    //k_msleep(50);
    //gpio_pin_set(user_led.port, user_led.pin, 0);
}

void main(void) {
    // init
    //printk("led init\n");
    //led_init();
    //printk("bt init\n");
    //command_init();
    printk("Adress first %p\n", gatt_iface.send_command_func);
    peripheral_init();
    printk("Adress after %p\n", gatt_iface.send_command_func);
    //serial_init();

    // blink led when starting
    //printk("blink\n");
    //blink();

    //printk("try to add command\n");
	//struct command_data command;
	//uint8_t head = 15;
	//command.key = ((head & 0xE0) >> 5); // read the 3 MSB from head
	//command.id = head & 0x1F;           // read the 5 LSB from head
	//command.value = 2;

	//printk("save new command\n");
	// Add new command to incoming commands
	//printk("added command to queue\n");

    printk("start waiting for command\n");
    struct command_data command;
    while (get_command(&command) == 0) {
        printk("Key: %d, id: %d, value: %d\n", command.key, command.id, command.value);
        printk("Sending command back\n");
        if (gatt_iface.send_command_func == NULL) {
            printk("No function stored\n");
        }
        else {
            (*gatt_iface.send_command_func)(&command, &gatt_iface);
        }
        printk("Sent command back.\n");
        /*
        switch (command.key) {
            case error_command: // an error ocurred!
                // acknowledge command, even though an error occurred
                //k_event_set(&ack_event, (1 << command.id));
                break;

            case ack_command: // set ack event when receiving ack
                //k_event_set(&ack_event, (1 << command.id));
                break;

            case ping_command: // received ping
                // ack and quit if ping is directed for me
                //if (command.value == ping_me) {
                //    ack_uart(&command, 0);
                //}
                break;
            
            default: // unrecognized command
                //error_uart(&command, error_unrecognized);
                break;
        }
        */
    }
}