#ifndef COMMAND_H
#define COMMAND_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

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
};

/** @brief Enumerated interfaces for ping command */
enum pings {
    /* Send ping to transmitter */
    ping_transmitter,
    /* Send ping to robot */
    ping_robot,
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

/** @brief Struct used for sending commands, independent of interface */
struct command_writer;
/** @brief Structure containing all data in commands */
struct command_data;

struct command_writer {
    /* Pointer to function for sending command */
    int (*send_command_func)(struct command_data);
    /* Pointer to communication interface */
    void *iface;
};

struct command_data {
    /* Command key from commands enum (3 bits) */
    uint8_t key;
    /* ID used in ack (5 bits) */
    uint8_t id;
    /* Value used in command (8 bits) */
    uint8_t value;
    /* Pointer to communication interface where command will be sent */
    struct command_writer *writer;
};

/**
 * @brief Get an id for next command
 *
 * @return id
 */
uint8_t get_message_id();

/**
 * @brief Set ack event when receiving answer
 *
 * @param id Id of message that was received
 */
void set_ack(uint8_t id);

/**
 * @brief Wait for acknowledgment of id
 *
 * @param id Id of message that are awaited
 * @param receive_time Pointer where receive time is saved
 * @return 0 if ack received, 1 if timeout
 */
int wait_for_ack(uint8_t id, int64_t *receive_time);

/**
 * @brief Add command to command queue
 *
 * @param command
 */
void add_command(struct command_data command);

/**
 * @brief Get next command from the command queue
 *
 * @param command Pointer to where command will be saved
 *
 * @return 0 if command received, negative value if error
 */
int get_command(struct command_data *command);

/**
 * @brief Send command over command writer specified in command
 * 
 * @param command Command that will be sent
 * @return Value from send handler. Return negative value on error
 */
int send_command(struct command_data command);

/**
 * @brief Send acknowledgement command
 *
 * @param command Command that should be acknowledged
 * @param receive_time Time it took from sending command to receiving ack
 */
void send_ack(struct command_data command, uint8_t receive_time);

/**
 * @brief Send error
 *
 * @param command Command contatining data
 * @param error_code Error code corresponding to enum error_codes
 */
void send_error(struct command_data command, uint8_t error_code);

/**
 * @brief Send ping command to specified interface and wait for ack
 *
 * @param writer Pointer to command writer, that handles sending the command
 *
 * @return id of new ping message
 */
uint8_t send_ping(struct command_writer *writer);

#endif