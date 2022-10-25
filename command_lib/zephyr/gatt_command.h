#ifndef GATT_COMMAND_H
#define GATT_COMMAND_H

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>

#include "command.h"

#define COMMAND_LEN 2
static uint8_t command_value[COMMAND_LEN];

/**
 * @brief Function for notifying commands over BLE GATT
 *
 * @param command Command that will be notified
 * @return int Return 0 if succesfull and negative value on error
 */
int send_command_gatt(struct command_data command);

/**
 * @brief Function for initializing Bluetooth and starting command service
 *
 */
int peripheral_init();

#endif