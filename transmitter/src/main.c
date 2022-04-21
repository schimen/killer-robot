#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <kernel.h>
#include <sys/arch_interface.h>
#include <zephyr.h>

#include "interfaces.h"

#define NUM_THREADS 10
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

// Set up for multiple threads
K_THREAD_STACK_EXTERN(stack);
K_THREAD_STACK_ARRAY_DEFINE(stacks, NUM_THREADS, STACK_SIZE);

static struct k_thread threads[NUM_THREADS];

// leds
const struct gpio_dt_spec user_led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec green_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(green_led), gpios);
const struct gpio_dt_spec red_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(red_led), gpios);

/**
 * @brief Initialize leds
 */
void led_init() {
    gpio_pin_configure_dt(&user_led, GPIO_OUTPUT_INACTIVE);
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
 * @brief Callback for threads processing incoming commands on transmitter
 *
 * Process incoming commands to the transmitter and respond to them.
 * This function is meant to be used for threads processing incoming and
 * outgoing commands in parallell.
 *
 * @param p1 temporarily used as a "thread-id", add pointer to long
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
    while (k_msgq_get(&incoming_commands, &command, K_FOREVER) == 0) {
        switch (command.key) {
            case error_command: // an error ocurred!
                // acknowledge command, even though an error occurred
                k_event_set(&ack_event, (1 << command.id));
                // send all errors to usb
                send_command(&usb, &command);
                blink(&red_led);
                break;

            case ack_command: // set ack event when receiving ack
                k_event_set(&ack_event, (1 << command.id));

            case ping_command: // send ping
                // ack and quit if ping is directed for me
                if (command.value == ping_me) {
                    ack(&command, 0);
                    blink(&green_led);
                    break;
                }
                // send ping and wait for ack
                if (wait_for_ack(ping(&command), &receive_time)) {
                    // message timed out
                    error(&command, error_timeout);
                    blink(&red_led);
                } 
                else {
                    // reply that message was succesfully delivered
                    ack(&command, (uint8_t)receive_time);
                    blink(&green_led);
                }
                break;

            case default_interface_command: // set default interface
                if (set_default_interface(command.value)) {
                    // interface number not recognized
                    error(&command, error_value);
                    blink(&red_led);

                }
                else {
                    // default interface set succesfully
                    ack(&command, 0);
                    blink(&green_led);
                }
                break;

            default: // unrecognized command, send it to robot
                send_command(default_iface, &command);
                // send message and wait for ack
                if (wait_for_ack(ping(&command), &receive_time)) {
                    // message timed out
                    error(&command, error_timeout);
                    blink(&red_led);
                } 
                else {
                    // reply that message was succesfully delivered
                    ack(&command, (uint8_t)receive_time);
                    blink(&green_led);
                }
                break;
        }
    }
}

void main(void) {
    // init
    led_init();
    serial_init();

    // set up threads
    for (uint8_t i = 0; i < NUM_THREADS; i++) {
        k_thread_create(&threads[i], stacks[i], STACK_SIZE, thread_callback,
                        NULL, NULL, NULL, THREAD_PRIORITY, K_USER, K_NO_WAIT);
    }

    // blink led when starting
    blink(&red_led);
    blink(&green_led);
}
