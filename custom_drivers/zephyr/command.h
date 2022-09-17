#ifndef COMMAND_H
#define COMMAND_H

#include <zephyr.h>
#include <kernel.h>

#define ACK_TIMEOUT K_MSEC(250)

#define MSG_SIZE 4
#define MAX_MSGS 10

/** @brief Enumerated command constants */
enum commands {
    /* Indicates that an error occurred */
    error_command,
    /* Acknowledge a received command */
    ack_command,
    /* Send ping command to specified interface */
    ping_command,
    /* Set speed of left motor */
    left_motor_command,
    /* Set speed of right motor */
    right_motor_command,
    /* Set speed of weapon motor */
    weapon_motor_command,
    /* Set default interface */
    default_interface_command
};

/** @brief Enumerated interfaces for ping command */
enum pings {
    /* Ping was sent to me, I must ack */
    ping_me,
    /* Send ping to USB interface */
    ping_usb,
    /* Send ping to HC-05 interface */
    ping_hc05,
    /* Send ping to HC-12 interface */
    ping_hc12
};

/** @brief Enumerated error codes for commands */
enum errors {
    /* Unrecognized command */
    error_unrecognized,
    /* Message was not acked before timeout */
    error_timeout,
    /* Message was parsed incorrectly */
    error_parse,
    /* Value in command not valid */
    error_value
};

/** @brief Structure containing all data in commands */
struct command_data {
    /* Command key from commands enum (3 bits) */
    uint8_t key;
    /* ID used in ack (5 bits) */
    uint8_t id;
    /* Value used in command (8 bits) */
    uint8_t value;
    /* Interface used to send command data */
    void* iface;
};

/**
 * @brief Get an id for next command
 *
 * @return id
 */
uint8_t get_message_id();

/**
 * @brief Wait for acknowledgment of id
 * 
 * @param id Id of message that are awaited
 * @param receive_time Pointer where receive time is saved
 * @return 0 if ack received, 1 if timeout 
 */
int wait_for_ack(uint8_t id, int64_t *receive_time);

void add_command(struct command_data* command);

int get_command(struct command_data* command);

#endif