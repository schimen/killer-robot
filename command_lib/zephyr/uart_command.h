/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef UART_COMMAND_H
#define UART_COMMAND_H

#include <string.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#include "command.h"

#define COMMAND_START '%'
#define COMMAND_END '&'

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
    /* UART device */
    const struct device *dev;
    /* Pin for setting AT commands */
    const struct gpio_dt_spec *state_pin;
    /* Receive buffer used in interrupt */
    struct uart_receive_buffer rx_buf;
    /* Mutex for reserving interface */
    struct k_mutex mutex;
};

/**
 * @brief Inititialize serial interface and command writer
 *
 * @param iface Pointer to uninitialized serial_interface
 * @param writer Pointer to unitialized command_writer
 * @param dev Pointer to serial_device
 * @param state_pin Pointer to pin used for state in hc devices.
 * Set NULL if not used.
 */
void serial_init(struct serial_interface *iface, struct command_writer *writer,
                 const struct device *dev,
                 const struct gpio_dt_spec *state_pin);

/**
 * @brief Enable state-pin and uart interrupt for serial interface
 * 
 * @param iface Pointer to serial_interface
 */
void serial_enable(struct serial_interface *iface);

/**
 * @brief Disable state-pin and uart interrupt for serial interface
 * 
 * @param iface Pointer to serial_interface
 */
void serial_disable(struct serial_interface *iface);

/**
 * @brief Print string to uart device (using polling)
 *
 * @param dev uart device
 * @param format String and format like used in f. ex printf
 */
void uart_print(const struct device *dev, const char *format, ...);

/**
 * @brief Send command over uart (using polling)
 *
 * @param command Command data to be sent
 */
int send_command_uart(struct command_data command);

/**
 * @brief UART interrupt handler receiving commands
 *
 * This function should be used in UART ISR.
 * It receives characters and waits for received commands.
 * If it receives a command, it is added to the command queue.
 *
 * @param dev Receiving UART device
 * @param user_data command_writer is passed to user_data. This should be the
 * command writer of the receiveing interface
 */
void receive_command_uart(const struct device *dev, void *user_data);

#endif /* INTERFACES_H */
