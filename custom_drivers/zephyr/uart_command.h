/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef UART_COMMAND_H
#define UART_COMMAND_H

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <kernel.h>
#include <string.h>
//#include <usb/usb_device.h>
#include <zephyr.h>

#include "command.h"

#define COMMAND_START ':'
#define COMMAND_END ';'


/** @brief Structure containing buffer, position and flag for receiving commands
 */
struct uart_receive_buffer {
    /* Array for storing incoming chars */
    unsigned char buffer[MSG_SIZE];
    /* Position of currenct received character in buffer */
    uint8_t position;
    /* Flag indicating if incoming char is part of new command */
    bool incoming;
};

/** @brief Structure containing necessary parts of serial interfaces */
struct serial_interface {
	/* Pointer to function for sending command */
	void (*send_command_func)(struct command_data*, void*);
    /* UART device */
    const struct device *dev;
    /* Pin for setting AT commands */
    const struct gpio_dt_spec *state_pin;
    /* Receive buffer used in interrupt */
    struct uart_receive_buffer *rx_buf;
    /* Mutex for reserving interface */
    struct k_mutex *mutex;
};

/**
 * @brief Print string to uart device (using polling)
 *
 * @param dev uart device
 * @param format String and format like used in f. ex printf
 */
void uart_print(const struct device *dev, const char *format, ...);

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
struct command_data parse_command_uart(unsigned char *command_string);

/**
 * @brief Send command over uart (using polling)
 *
 * @param command Pointer to command_data
 * @param iface_ptr Pointer to serial_interface
 */
int send_command_uart(struct command_data *command, void *iface_ptr);

/**
 * @brief Send acknowledgement command
 *
 * @param command Command that should be acknowledged
 * @param receive_time Time it took from sending command to receiving ack
 */
void ack_uart(struct command_data *command, uint8_t receive_time);

/**
 * @brief Send error
 *
 * @param command Command contatining data
 * @param error_code Error code corresponding to enum error_codes
 */
void error_uart(struct command_data *command, uint8_t error_code);

/**
 * @brief Send ping command to specified interface and wait for ack
 *
 * @param command The received ping command
 *
 * @return id of new ping message
 */
uint8_t ping_uart(struct command_data *command);

/**
 * @brief Set the default interface according according to ping interface enum
 *
 * @param interface_number The number of interface, according to pings enum
 *
 * @return 0 if succesfull, 1 if interface not valid
 */
uint8_t set_default_uart_interface(uint8_t interface_number);

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
void receive_command_uart(const struct device *dev, void *user_data);

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
extern struct serial_interface *default_uart_iface;

#endif /* INTERFACES_H */
