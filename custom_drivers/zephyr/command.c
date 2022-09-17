#include "command.h"

//static struct k_msgq incoming_commands;
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

void add_command(struct command_data* command)
{
    printk("Adding command\n");
	// Add new command to incoming commands
	while (k_msgq_put(&incoming_commands, command, K_NO_WAIT) != 0) {
		// message queue is full, purge and try again
        printk("queue full\n");
		k_msgq_purge(&incoming_commands);
	}
	printk("Added command to queue\n");
}

int get_command(struct command_data* command) {
    return k_msgq_get(&incoming_commands, command, K_FOREVER);
}