/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef INTERFACES_H
#define INTERFACES_H

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <kernel.h>
#include <usb/usb_device.h>
#include <zephyr.h>

#define ACK_TIMEOUT K_MSEC(250)

#define MSG_SIZE 4
#define MAX_MSGS 10

#define COMMAND_START ':'
#define COMMAND_END ';'

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

/** @brief Structure containing buffer, position and flag for receiving commands
 */
struct receive_buffer {
    /* Array for storing incoming chars */
    unsigned char buffer[MSG_SIZE];
    /* Position of currenct received character in buffer */
    uint8_t position;
    /* Flag indicating if incoming char is part of new command */
    bool incoming;
};

/** @brief Structure containing necessary parts of serial interfaces */
struct serial_interface {
    /* UART device */
    const struct device *dev;
    /* Pin for setting AT commands */
    const struct gpio_dt_spec *state_pin;
    /* Receive buffer used in interrupt */
    struct receive_buffer *rx_buf;
    /* Mutex for reserving interface */
    struct k_mutex *mutex;
};

/** @brief Structure containing all data in commands */
struct command_data {
    /* Command key from commands enum (3 bits) */
    uint8_t key;
    /* ID used in ack (5 bits) */
    uint8_t id;
    /* Value used in command (8 bits) */
    uint8_t value;
    /* Interface that sent this command */
    struct serial_interface *iface;
};

/**
 * @brief Print string to uart device (using polling)
 *
 * @param dev uart device
 * @param format String and format like used in f. ex printf
 */
void uart_print(const struct device *dev, const char *format, ...);

/**
 * @brief Get an id for next command
 *
 * @return id
 */
uint8_t get_message_id();

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
struct command_data parse_command(unsigned char *command_string);

/**
 * @brief Send command over uart (using polling)
 *
 * @param iface Pointer to serial_interface
 * @param handler Pointer to command_data
 */
int send_command(struct serial_interface *iface, struct command_data *command);

/**
 * @brief Wait for acknowledgment of id
 * 
 * @param id Id of message that are awaited
 * @param receive_time Pointer where receive time is saved
 * @return 0 if ack received, 1 if timeout 
 */
int wait_for_ack(uint8_t id, int64_t *receive_time);

/**
 * @brief Send acknowledgement command
 *
 * @param command Command that should be acknowledged
 * @param receive_time Time it took from sending command to receiving ack
 */
void ack(struct command_data *command, uint8_t receive_time);

/**
 * @brief Send error
 *
 * @param command Command contatining data
 * @param error_code Error code corresponding to enum error_codes
 */
void error(struct command_data *command, uint8_t error_code);

/**
 * @brief Send ping command to specified interface and wait for ack
 *
 * @param command The received ping command
 *
 * @return id of new ping message
 */
uint8_t ping(struct command_data *command);

/**
 * @brief Set the default interface according according to ping interface enum
 *
 * @param interface_number The number of interface, according to pings enum
 *
 * @return 0 if succesfull, 1 if interface not valid
 */
uint8_t set_default_interface(uint8_t interface_number);

/**
 * @brief UART interrupt handler receiving commands
 *
 * This function should be used in UART ISR.
 * It receives characters and waits for received commands.
 * If it receives a command, it is added to the command queue.
 *
 * @param dev Receiving UART device
 * @param user_data serial_interface is passed to user_data. This should be the
 * receiving serial interface
 */
void receive_command(const struct device *dev, void *user_data);

/**
 * @brief Initialize serial devices (usb and uart)
 */
void serial_init();

/* Serial interface for usb cdc acm */
extern struct serial_interface usb;
/* Serial Interface for HC-05 bluetooth transceiver */
extern struct serial_interface hc05;
/* Serial interface for HC-12 433MHz transceiver */
extern struct serial_interface hc12; 
/* Default serial interface pointer */
extern struct serial_interface *default_iface;
/* Message queue for incoming commands */
extern struct k_msgq incoming_commands;
/* Event for acknowledgments */
extern struct k_event ack_event;

#endif /* INTERFACES_H */
