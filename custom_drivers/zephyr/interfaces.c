#include "interfaces.h"

// define global variables declared in header file
struct serial_interface usb;
struct serial_interface hc05;
struct serial_interface hc12; 
struct serial_interface *default_iface;
struct k_msgq incoming_commands;
struct k_event ack_event;

// hc set pins
const struct gpio_dt_spec hc05_set =
    GPIO_DT_SPEC_GET(DT_NODELABEL(hc05_set), gpios);
const struct gpio_dt_spec hc12_set =
    GPIO_DT_SPEC_GET(DT_NODELABEL(hc12_set), gpios);

// serial devices
const struct device *usb_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
const struct device *hc05_device = DEVICE_DT_GET(DT_ALIAS(hc05));
const struct device *hc12_device = DEVICE_DT_GET(DT_ALIAS(hc12));

// Mutex for serial interfaces
K_MUTEX_DEFINE(usb_mutex);
K_MUTEX_DEFINE(hc05_mutex);
K_MUTEX_DEFINE(hc12_mutex);

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

uint8_t get_message_id() {
    static uint8_t message_id = 0;
    if (message_id >= 32) {
        message_id = 0;
    } else {
        message_id++;
    }
    return message_id;
}

struct command_data parse_command(unsigned char *command_string) {
    struct command_data command;
    if (command_string[0] == COMMAND_START &&
        command_string[3] == COMMAND_END) {
        uint8_t key, id, head;

        head = command_string[1];
        key = ((head & 0xE0) >> 5); // read the 3 MSB from head
        id = head & 0x1F;           // read the 5 LSB from head

        command.key = key;
        command.id = id;
        command.value = command_string[2];
    }

    else {
        // error, everything is set to zero
        command.key = error_command;
        command.id = get_message_id();
        command.value = error_parse;
    }

    return command;
}

int send_command(struct serial_interface *iface, struct command_data *command) {
    // return k_msgq_put(iface->outgoing, buffer, K_NO_WAIT);
    const struct device *dev = iface->dev;
    // create head with key in 3 MSB and id in 5 LSB
    uint8_t head = (command->key << 5) | command->id;

    // lock mutex before sending
    if (k_mutex_lock(iface->mutex, K_MSEC(100))) {
        // if mutex is not available in 100ms
        return 1;
    }
    // send command
    uart_poll_out(dev, COMMAND_START);
    uart_poll_out(dev, head);
    uart_poll_out(dev, command->value);
    uart_poll_out(dev, COMMAND_END);
    k_mutex_unlock(iface->mutex); // unlock mutex after sending
    return 0;
}

int wait_for_ack(uint8_t id, int64_t *receive_time) {
    int64_t start_time = k_uptime_get();
    if (k_event_wait(&ack_event, (1 << id), true, ACK_TIMEOUT) == 0) {
        return 1;
    }
    // Compute passed time in milliseconds
    *receive_time = k_uptime_delta(&start_time);
    if (*receive_time > 255) {
        // time is out of bounds for 8 bit value in command
        *receive_time = 255;
    }
    return 0;
}

void ack(struct command_data *command, uint8_t receive_time) {
    struct command_data new_command;
    new_command.key = ack_command;
    new_command.id = command->id;
    new_command.value = receive_time;
    send_command(command->iface, &new_command);
}

void error(struct command_data *command, uint8_t error_code) {
    struct command_data new_command;
    new_command.key = error_command;
    new_command.id = command->id;
    new_command.value = error_code;
    send_command(command->iface, &new_command);
}

uint8_t ping(struct command_data *command) {
    // Choose interface to ping, based on command value
    struct serial_interface *iface = NULL;
    switch (command->value) {
    case ping_usb:
        iface = &usb;
        break;
    case ping_hc05:
        iface = &hc05;
        break;
    case ping_hc12:
        iface = &hc12;
        break;
    default:
        iface = default_iface;
    }
    struct command_data new_command;
    new_command.key = ping_command;
    new_command.id = get_message_id();
    new_command.value = 0;
    send_command(iface, &new_command);
    return new_command.id;
}

uint8_t set_default_interface(uint8_t interface_number) {
    switch (interface_number) {
        case ping_hc05:
            default_iface = &hc05;
            break;
        case ping_hc12:
            default_iface = &hc12;
            break;
        default: // interface number not recognized
            return 1;
            break;
    }
    return 0;
}

void receive_command(const struct device *dev, void *user_data) {
    if (!uart_irq_update(dev)) {
        return;
    }

    uint8_t c;
    struct serial_interface *iface = (struct serial_interface *)user_data;
    struct receive_buffer *rx_buf = iface->rx_buf;

    if (uart_irq_rx_ready(dev)) { // this is very error prone atm
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
                    struct command_data command = parse_command(rx_buf->buffer);

                    // Save interface that sent command
                    command.iface = iface;
                    // send to queue for processing
                    while (k_msgq_put(&incoming_commands, &command,
                                    K_NO_WAIT) != 0) {
                        // message queue is full, purge and try again
                        k_msgq_purge(&incoming_commands);
                    }
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

void serial_init() {
    // enable usb
    usb_enable(NULL);

    // set up receive buffers
    static struct receive_buffer usb_rx;
    static struct receive_buffer hc05_rx;
    static struct receive_buffer hc12_rx;

    // set up hc set pins
    gpio_pin_configure_dt(&hc05_set, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&hc12_set, GPIO_OUTPUT_INACTIVE);

    // set up serial interfaces
    usb.dev = usb_device;
    usb.state_pin = NULL;
    usb.rx_buf = &usb_rx;
    usb.mutex = &usb_mutex;

    hc05.dev = hc05_device;
    hc05.state_pin = &hc05_set;
    hc05.rx_buf = &hc05_rx;
    hc05.mutex = &hc05_mutex;

    hc12.dev = hc12_device;
    hc12.state_pin = &hc12_set;
    hc12.rx_buf = &hc12_rx;
    hc12.mutex = &hc12_mutex;

    // set default interface
    default_iface = &hc05;

    // set up callbacks
    uart_irq_callback_user_data_set(usb.dev, receive_command, &usb);
    uart_irq_callback_user_data_set(hc05.dev, receive_command, &hc05);
    uart_irq_callback_user_data_set(hc12.dev, receive_command, &hc12);

    // enable callbacks
    uart_irq_rx_enable(usb.dev);
    uart_irq_rx_enable(hc05.dev);
    uart_irq_rx_enable(hc12.dev);

    // init event 
    k_event_init(&ack_event);

    // init message queue
    uint8_t data_size = sizeof(struct command_data);
    char __aligned(4) msgq_buffer[MAX_MSGS * data_size];
    k_msgq_init(&incoming_commands, msgq_buffer, data_size, MAX_MSGS);
}