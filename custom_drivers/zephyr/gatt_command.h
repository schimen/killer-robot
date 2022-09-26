#ifndef GATT_COMMAND_H
#define GATT_COMMAND_H

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "command.h"

#define COMMAND_LEN 2
static uint8_t command_value[COMMAND_LEN];

/**
 * @brief Function for initializing Bluetooth and starting command service
 * 
 */
int peripheral_init();

#endif