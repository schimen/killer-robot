#ifndef GATT_COMMAND_H
#define GATT_COMMAND_H

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "command.h"

#define COMMAND_LEN 2
static uint8_t command_value[COMMAND_LEN];

/** @brief BLE interface for sending commands */
struct gatt_interface {
	/* Pointer to function for sending command */
	void (*send_command_func)(struct command_data*, void*);
    /* BT connection */
    struct bt_conn *conn;
};

static struct gatt_interface gatt_iface;

void send_command_gatt(struct command_data* command, void* iface_ptr);

void connected(struct bt_conn *conn, uint8_t err);

void disconnected(struct bt_conn *conn, uint8_t reason);

void bt_ready(void);

void auth_passkey_display(struct bt_conn *conn, unsigned int passkey);

void auth_cancel(struct bt_conn *conn);

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx);

void peripheral_init();

#define BT_UUID_CUSTOM_SERVICE_VAL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
static struct bt_uuid_128 command_uuid = BT_UUID_INIT_128(
	BT_UUID_CUSTOM_SERVICE_VAL
);
static struct bt_uuid_128 command_value_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)
);
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
};

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static struct bt_gatt_cb gatt_callbacks = {
	.att_mtu_updated = mtu_updated
};

#endif