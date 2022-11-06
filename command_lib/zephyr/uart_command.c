#include "uart_command.h"

// Use logger from command.c
LOG_MODULE_DECLARE(command);

void serial_init(struct serial_interface *iface, struct command_writer *writer,
                 const struct device *dev,
                 const struct gpio_dt_spec *state_pin) {
    // Create iface object
    iface->dev = dev;
    iface->state_pin = state_pin;
    // Initialize mutex
    k_mutex_init(&iface->mutex);
    // Set up writer
    writer->send_command_func = &send_command_uart;
    writer->iface = iface;
    // Set up and enable uart interrupt
    uart_irq_callback_user_data_set(iface->dev, receive_command_uart, writer);
    serial_enable(iface);
}

void serial_enable(struct serial_interface *iface) {
    // Configure state pin
    if (iface->state_pin) {
        gpio_pin_configure_dt(iface->state_pin, GPIO_OUTPUT_INACTIVE);
    }
    // Enable uart interrupt
    uart_irq_rx_enable(iface->dev);
}

void serial_disable(struct serial_interface *iface) {
    // Configure state pin
    if (iface->state_pin) {
        gpio_pin_configure_dt(iface->state_pin, GPIO_DISCONNECTED);
    }
    // Enable uart interrupt
    uart_irq_rx_disable(iface->dev);
}

void uart_print(const struct device *dev, const char *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    char out[512];
    vsnprintk(out, 512, format, argptr);

    int msg_len = strlen(out);
    for (int i = 0; i < msg_len; i++) {
        uart_poll_out(dev, out[i]);
    }
    va_end(argptr);
}

/**
 * @brief Parse command string to command
 *
 * Parse string received via uart.
 * Check that command structure is followed and put values in new command
 *
 * @param command_string String containing command
 *
 * @return command
 */
struct command_data parse_command_uart(struct command_writer *writer,
                                       unsigned char *command_string) {
    struct command_data command;
    command.writer = writer;

    if (command_string[0] == COMMAND_START &&
        command_string[3] == COMMAND_END) {
        uint8_t key, id, head;

        head = command_string[1];
        key = ((head & 0xE0) >> 5); // read the 3 MSB from head
        id = head & 0x1F;           // read the 5 LSB from head

        command.key = key;
        command.id = id;
        command.value = command_string[2];
    } else {
        // error, everything is set to zero
        command.key = error_command;
        command.id = get_message_id();
        command.value = error_parse;
    }
    return command;
}

int send_command_uart(struct command_data command) {
    struct serial_interface *iface = command.writer->iface;
    // return k_msgq_put(iface->outgoing, buffer, K_NO_WAIT);
    const struct device *dev = iface->dev;
    // create head with key in 3 MSB and id in 5 LSB
    uint8_t head = (command.key << 5) | command.id;

    // lock mutex before sending
    if (k_mutex_lock(&iface->mutex, K_MSEC(100))) {
        // if mutex is not available in 100ms
        LOG_WRN("Warning: Serial interface mutex lock timed out");
        return 1;
    }
    // send command
    uart_poll_out(dev, COMMAND_START);
    uart_poll_out(dev, head);
    uart_poll_out(dev, command.value);
    uart_poll_out(dev, COMMAND_END);
    k_mutex_unlock(&iface->mutex); // unlock mutex after sending
    return 0;
}

void receive_command_uart(const struct device *dev, void *user_data) {
    if (!uart_irq_update(dev)) {
        return;
    }
    if (uart_irq_rx_ready(dev)) {
        uint8_t c;
        struct command_writer *writer = user_data;
        struct serial_interface *iface = writer->iface;
        struct uart_receive_buffer *rx_buf = &iface->rx_buf;
        while (uart_fifo_read(dev, &c, 1)) {
            if (c == COMMAND_START) {
                // start of command
                rx_buf->incoming = true;
            }
            if (rx_buf->incoming) {
                // buffer not yet full
                rx_buf->buffer[rx_buf->position] = c;

                if (c == COMMAND_END &&
                    rx_buf->position == 3) { // command is finished
                    // send message to incoming queue
                    struct command_data command =
                        parse_command_uart(writer, rx_buf->buffer);

                    // send to queue for processing
                    add_command(command);
                }
                if (rx_buf->position >= MSG_SIZE - 1) {
                    // reset when position is MSG_SIZE or larger
                    rx_buf->incoming = false;
                    rx_buf->position = 0;
                    memset(rx_buf->buffer, 0, sizeof(rx_buf->buffer));
                } else {
                    rx_buf->position++;
                }
                // all characters are dropped after position is larger than
                // message size
            }
            // all characters not part of command dropped
        }
    }
}
