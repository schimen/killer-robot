#include "command.h"

// Create incoming command queue and ack event for acks
K_MSGQ_DEFINE(incoming_commands, sizeof(struct command_data), MAX_MSGS, 4);
K_EVENT_DEFINE(ack_event);

uint8_t get_message_id() {
    static uint8_t message_id = 0;
    if (message_id >= 32) {
        message_id = 0;
    } else {
        message_id++;
    }
    return message_id;
}

void set_ack(uint8_t id) {
    k_event_set(&ack_event, (1 << id));
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

void add_command(struct command_data command) {
	// Add new command to incoming commands
	while (k_msgq_put(&incoming_commands, &command, K_NO_WAIT) != 0) {
		// message queue is full, purge and try again
		k_msgq_purge(&incoming_commands);
	}
}

int get_command(struct command_data* command) {
    return k_msgq_get(&incoming_commands, command, K_FOREVER);
}

void send_command(struct command_data command) {
    if (command.writer == NULL) {
        printk("No writer for this interface");
        return;
    }
    if (command.writer->send_command_func) {
        (command.writer->send_command_func)(command);
    }
    else {
        printk("No function for this writer\n");
    }
}

void send_ack(struct command_data command, uint8_t receive_time) {
    command.key = ack_command;
    command.value = receive_time;
    send_command(command);
}

void send_error(struct command_data command, uint8_t error_code) {
    command.key = error_command;
    command.value = error_code;
    send_command(command);
}

uint8_t send_ping(struct command_writer *writer) {
    struct command_data command;
    command.key = ping_command;
    command.id = get_message_id();
    command.value = 0;
    send_command(command);
    return command.id;
}