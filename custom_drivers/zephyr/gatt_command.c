#include "gatt_command.h"

static struct command_data new_command;

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	printk("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
}

ssize_t write_command(
	struct bt_conn *conn,
	const struct bt_gatt_attr *attr,
	const void *buf,
	uint16_t len,
	uint16_t offset,
	uint8_t flags
) {

	printk("writing command\n");
	uint8_t *value = attr->user_data;

	if (offset + len > COMMAND_LEN) {
		printk("Invalid offset (offset: %d, len: %d)", offset, len);
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	printk("correct offset and len\n");

	memcpy(value + offset, buf, len);
	printk("saved new value\n");
	printk("copied new value\n");
	uint8_t head = value[0];
	new_command.key = ((head & 0xE0) >> 5); // read the 3 MSB from head
	new_command.id = head & 0x1F;           // read the 5 LSB from head
	new_command.value = value[1];

	add_command(&new_command);
	printk("Finished write command\n");

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

void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
		return;
	}
	printk("Connected\n");
	gatt_iface.conn = conn;
}

void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

void bt_ready(void)
{
	int err;

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Passkey for %s: %06u\n", addr, passkey);
}

void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

void send_command_gatt(struct command_data* command, void* iface_ptr) {
	printk("test command pointer");
	//printk("send command gatt");
	//struct gatt_interface* iface = iface_ptr;
	//int err = bt_gatt_notify_uuid(iface->conn, &command_value_uuid.uuid, NULL, &command_value, 2);
	//if (err) {
	//	printk("notification error\n");
	//}
	//printk("notified connection\n");
}


void peripheral_init()
{
	int err;
	gatt_iface.send_command_func = &send_command;
	printk("%p\n", gatt_iface.send_command_func);
	printk("start bt\n");
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
	printk("bt started\n");
	bt_ready();
	printk("register callback gatt\n");
	bt_gatt_cb_register(&gatt_callbacks);
	printk("register callback auth\n");
	bt_conn_auth_cb_register(&auth_cb_display);
}

BT_GATT_SERVICE_DEFINE(command,
	BT_GATT_PRIMARY_SERVICE(&command_uuid),
	BT_GATT_CHARACTERISTIC(
		&command_value_uuid.uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY, 
		BT_GATT_PERM_WRITE | BT_GATT_PERM_READ,
		read_command, write_command, &command_value
	),
);