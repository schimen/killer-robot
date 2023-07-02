#include <command_lib/gatt_command.h>

// Use logger from command.c
LOG_MODULE_DECLARE(command);

#define BT_UUID_CUSTOM_SERVICE_VAL                                             \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

struct command_writer default_gatt_writer = {
    .iface = NULL, .send_command_func = &send_command_gatt};

static struct bt_uuid_128 command_uuid =
    BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_VAL);
static struct bt_uuid_128 command_write_uuid = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1));

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

// Callback function for new connection
static void connected(struct bt_conn *conn, uint8_t err) {
    // Return upon error
    if (err) {
        LOG_ERR("Error %d: Failed to connect", err);
        return;
    }
#if CONFIG_LOG_DEFAULT_LEVEL >= LOG_LEVEL_INF
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Connected to %s", addr);
#endif
    // Request new connection parameters
    // (connection interval between 15 and 30 ms and supervision timeout at 4s)
    const struct bt_le_conn_param *param = BT_LE_CONN_PARAM(12, 24, 0, 400);
    bt_conn_le_param_update(conn, param);
}

// Callback function for disconnection
static void disconnected(struct bt_conn *conn, uint8_t reason) {
#if CONFIG_LOG_DEFAULT_LEVEL >= LOG_LEVEL_INF
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Disconnected from %s (reason: %d)", addr, reason);
#endif
}

// Register connection callback functions
BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

/**
 * @brief Callback function called every time a new commands are incoming on
 * command characteristic.
 */
static ssize_t write_command(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr, const void *buf,
                             uint16_t len, uint16_t offset, uint8_t flags) {
    // Only accept write without response requests
    if (!(flags & BT_GATT_WRITE_FLAG_CMD)) {
        return BT_GATT_ERR(BT_ATT_ERR_WRITE_REQ_REJECTED);
    }
    // Error if invalid offset
    // (length and offset must make an even number)
    if ((offset + len) % 2 != 0) {
        LOG_ERR("Error %d: sum of length (%d) and offset (%d) must be even",
                BT_ATT_ERR_INVALID_OFFSET, len, offset);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    // Initialize head and command variables and buffer pointer
    uint8_t head;
    struct command_data command;
    command.writer = &default_gatt_writer;
    const uint8_t *data_buffer = buf;
    // Loop through all new messages (each message is 2 bytes)
    for (uint16_t i = 0; i < (len - 1); i += 2) {
        // Create command data from value
        head = data_buffer[i];
        command.key = ((head & 0xE0) >> 5); // read the 3 MSB from head
        command.id = head & 0x1F;           // read the 5 LSB from head
        command.value = data_buffer[i + 1];
        add_command(command);
    }
    return len;
}

/**
 * @brief Local function for setting iface-value to default-gatt iface
 * when connection subscribes to command characteristic.
 */
static void subscribe_attr_cb(const struct bt_gatt_attr *attr, uint16_t value);

BT_GATT_SERVICE_DEFINE(command_service, BT_GATT_PRIMARY_SERVICE(&command_uuid),
                       BT_GATT_CHARACTERISTIC(&command_write_uuid.uuid,
                                              BT_GATT_CHRC_WRITE_WITHOUT_RESP |
                                                  BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_WRITE, NULL,
                                              write_command, NULL),
                       BT_GATT_CCC(subscribe_attr_cb, BT_GATT_PERM_WRITE));

static void subscribe_attr_cb(const struct bt_gatt_attr *attr, uint16_t value) {
    // If chracterisitc is subscribed to, save as default attr
    if (value == BT_GATT_CCC_NOTIFY) {
        default_gatt_writer.iface = (void *)&command_service.attrs[1];
    }
}

int send_command_gatt(struct command_data command) {
    int err;
    const struct bt_gatt_attr *attr = command.writer->iface;
    if (attr) { // Send command if attribute exists
        uint8_t head = (command.key << 5) | (command.id & 0x07);
        uint8_t data[COMMAND_LEN] = {head, command.value};
        // Notify new command
        err = bt_gatt_notify(NULL, attr, &data, COMMAND_LEN);
        if (err) {
            LOG_ERR("Error %d: Failed to notify attribute", err);
        }
    } else { // Return error if attr is NULL
        LOG_WRN("Warning: Command writer iface not set");
        err = -1;
    }
    return err;
}

int peripheral_enable() {
    int err;
    // Enable bluetooth, return if error
    if (!bt_is_ready()) {
        err = bt_enable(NULL);
        if (err) {
            LOG_ERR("Error %d: failed to enable bluetooth", err);
            return -1;
        }
    }
    // Start bluetooth advertising, return if error
    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        LOG_ERR("Error %d: failed to start advertising", err);
        return -1;
    }
    return 0;
}

int peripheral_disable() {
    // Stop advertising
    return bt_le_adv_stop();
}
