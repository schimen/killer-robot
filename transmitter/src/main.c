#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/arch_interface.h>

#ifdef CONFIG_USB_DEVICE_STACK // Include usb if available
#include <zephyr/usb/usb_device.h>
#endif

#include "command.h"
#include "uart_command.h"

#define NUM_THREADS 10
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

// Create structs for hc12 interface
struct serial_interface hc12_iface;
struct command_writer hc12_writer;
const struct gpio_dt_spec hc12_set = GPIO_DT_SPEC_GET(DT_ALIAS(hc12set), gpios);
const struct device *hc12_device = DEVICE_DT_GET(DT_ALIAS(hc12));

// Create structs for usb interface
struct serial_interface usb_iface;
struct command_writer usb_writer;
const struct device *usb_device = DEVICE_DT_GET(DT_ALIAS(usb));


// Set up for multiple threads
K_THREAD_STACK_ARRAY_DEFINE(stacks, NUM_THREADS, STACK_SIZE);
static struct k_thread threads[NUM_THREADS];

// leds
const struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(DT_ALIAS(greenled), gpios);
const struct gpio_dt_spec red_led  = GPIO_DT_SPEC_GET(DT_ALIAS(redled), gpios);

/**
 * @brief Initialize leds
 */
void led_init() {
    gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_INACTIVE);
}

/**
 * @brief Blink led
 */
void blink(const struct gpio_dt_spec *led) {
    gpio_pin_set(led->port, led->pin, 1);
    k_msleep(50);
    gpio_pin_set(led->port, led->pin, 0);
}

/**
 * @brief Send ping command from the transmitter
 * 
 * @param command Command that triggered this function
 */
void ping_from_transmitter(struct command_data command) {
    int64_t receive_time;
    switch (command.value) {
        // ack and quit if ping is directed for me
        case ping_transmitter:
            send_ack(command, 0);
            blink(&green_led);
            break;
        
        case ping_robot:
            if (wait_for_ack(send_ping(&hc12_writer), &receive_time)) {
                // message timed out
                send_error(command, error_timeout);
                blink(&red_led);
                break;
            }
            // reply that message was succesfully delivered
            send_ack(command, (uint8_t)receive_time);
            blink(&green_led);
            break;
        default:
            send_error(command, error_value);
            break;
    }
}

/**
 * @brief Callback for threads processing incoming commands on transmitter
 *
 * Process incoming commands to the transmitter and respond to them.
 * This function is meant to be used for threads processing incoming and
 * outgoing commands in parallell.
 *
 * @param p1 Unused
 * @param p2 Unused
 * @param p3 Unused
 */
void thread_callback(void *p1, void *p2, void *p3) {
    // arguments are not used
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    // wait for new received commands
    int64_t receive_time;
    struct command_data command;
    struct command_data new_command;
    while (get_command(&command) == 0) {
        switch (command.key) {
            case error_command: // an error ocurred!
                // acknowledge command, even though an error occurred
                set_ack(command.id);
                // send all errors to usb
                command.writer = &usb_writer;
                send_command_uart(command);
                blink(&red_led);
                break;

            case ack_command: // set ack event when receiving ack
                set_ack(command.id);
                break;

            case ping_command: // send ping
                ping_from_transmitter(command);
                break;

            default: // unrecognized command, send it to robot
                new_command = command;
                new_command.writer = &hc12_writer;
                // send message and wait for ack
                send_command_uart(new_command);
                if (wait_for_ack(command.id, &receive_time)) {
                    // message timed out, send error back to original sender
                    send_error(command, error_timeout);
                    blink(&red_led);
                    break;
                } 
                // reply that message was succesfully delivered
                send_ack(command, (uint8_t)receive_time);
                blink(&green_led);
                break;
        }
    }
}

void main(void) {
    // init
    led_init();
    #ifdef CONFIG_USB_DEVICE_STACK // Enable usb if available
    usb_enable(NULL);
    #endif
    // Initialize serial interfaces
    serial_init(&hc12_iface, &hc12_writer, hc12_device, &hc12_set);
    serial_init(&usb_iface, &usb_writer, usb_device, NULL);

    // set up threads
    for (uint8_t i = 0; i < NUM_THREADS; i++) {
        k_thread_create(&threads[i], stacks[i], STACK_SIZE, thread_callback,
                        NULL, NULL, NULL, THREAD_PRIORITY, K_USER, K_NO_WAIT);
    }
    
    // blink led when starting
    blink(&red_led);
    blink(&green_led);
}