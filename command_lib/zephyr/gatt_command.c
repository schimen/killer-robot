#include "gatt_command.h"

#define BT_UUID_CUSTOM_SERVICE_VAL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)


struct command_writer default_gatt_writer = {
	.iface = NULL,
	.send_command_func = &send_command_gatt
};

static struct bt_uuid_128 command_uuid = BT_UUID_INIT_128(
	BT_UUID_CUSTOM_SERVICE_VAL
);
static struct bt_uuid_128 command_value_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)
);

int send_command_gatt(struct command_data command) {
	const struct bt_gatt_attr *attr = command.writer->iface;
	int err;
	if (attr) { // Send command if attribute exists
		uint8_t head = (command.key << 5) | (command.id & 0x07);
		uint8_t data[COMMAND_LEN] = { head, command.value };
		// Notify new command
		err = bt_gatt_notify(NULL, attr, &data, COMMAND_LEN);
	}
	else { // Return error if attr is NULL
		err = -1;
	}
	return err;
}

ssize_t write_command(
	struct bt_conn *conn,
	const struct bt_gatt_attr *attr,
	const void *buf,
	uint16_t len,
	uint16_t offset,
	uint8_t flags
) {
	// Pointer used to read values written to service
	uint8_t *value = attr->user_data;

	// Error if invalid offset
	if (offset + len > COMMAND_LEN) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Copy buffer to value
	memcpy(value + offset, buf, len);

	// Create command data from value
	uint8_t head = value[0];
	struct command_data command;
	command.key = ((head & 0xE0) >> 5); // read the 3 MSB from head
	command.id = head & 0x1F;           // read the 5 LSB from head
	command.value = value[1];
	command.writer = &default_gatt_writer;
	add_command(command);

	return len;
}

static ssize_t read_command(
	struct bt_conn *conn,
	const struct bt_gatt_attr *attr,
	void *buf,
	uint16_t len,
	uint16_t offset
) {
	const char *value = attr->user_data;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 strlen(value));
}

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

void bt_ready(void) {
	// Start bluetooth advertising
	bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
}

int peripheral_init() {
	// Enable bluetooth, return if error
	if (bt_enable(NULL)) {
		return -1;
	}
	// Start bluetooth
	bt_ready();
	return 0;
}

static void subscribe_attr_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
	// If chracterisitc is subscribed to, save as default attr
	if (value == BT_GATT_CCC_NOTIFY) {
		default_gatt_writer.iface = attr;
	}
}

BT_GATT_SERVICE_DEFINE(command,
	BT_GATT_PRIMARY_SERVICE(&command_uuid),
	BT_GATT_CHARACTERISTIC(
		&command_value_uuid.uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE, 
		BT_GATT_PERM_WRITE | BT_GATT_PERM_READ,
		read_command, write_command, &command_value
	),
	BT_GATT_CCC(
		subscribe_attr_cb,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
	),
);
