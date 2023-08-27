#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "icm20948.h"

// Use logger from icm20948.c
LOG_MODULE_DECLARE(ICM20948);

struct debug_data {
    int64_t time;
    struct icm20948_data data;
};

static struct debug_data debug_info = {0};
static const struct device *icm20948_device =
    DEVICE_DT_GET_ONE(invensense_icm20948);

// Declare functions so they are available for thread and service definition
static void subscribe_attr_cb(const struct bt_gatt_attr *attr, uint16_t value);
static ssize_t debug_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset);
static void notify_sensor_value(void *arg1, void *arg2, void *arg3);

#define BT_UUID_DEBUG_SERVICE_VAL                                              \
    BT_UUID_128_ENCODE(0xdeb12345, 0x1234, 0x5678, 0x1234, 0x56789abcdefa)

static struct bt_uuid_128 debug_uuid =
    BT_UUID_INIT_128(BT_UUID_DEBUG_SERVICE_VAL);
static struct bt_uuid_128 debug_info_uuid = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0xdeb12345, 0x1234, 0x5678, 0x1234, 0x56789abcdefb));

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_DEBUG_SERVICE_VAL),
};

BT_GATT_SERVICE_DEFINE(debug_service, BT_GATT_PRIMARY_SERVICE(&debug_uuid),
                       BT_GATT_CHARACTERISTIC(&debug_info_uuid.uuid,
                                              BT_GATT_CHRC_READ |
                                                  BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ, debug_read,
                                              NULL, &debug_info),
                       BT_GATT_CCC(subscribe_attr_cb, BT_GATT_PERM_WRITE));

K_THREAD_DEFINE(dbg_notify_tid, 1024, notify_sensor_value, NULL, NULL, NULL, 5,
                0, 0);

static void notify_sensor_value(void *arg1, void *arg2, void *arg3) {
    // Initialize thread
    const struct device *sensor_device = arg1;
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);
    struct bt_gatt_attr *attr = bt_gatt_find_by_uuid(
        debug_service.attrs, debug_service.attr_count, &debug_info_uuid.uuid);

    // Suspend thread until debug service is subscribed to
    k_thread_suspend(dbg_notify_tid);
    while (true) {
        if (!device_is_ready(icm20948_device)) {
            LOG_ERR("ICM20948 device is not ready");
            break;
        }
        sensor_sample_fetch(icm20948_device);
        memcpy(&debug_info.data, icm20948_device->data,
            sizeof(struct icm20948_data));
        debug_info.time = k_uptime_get();
        bt_gatt_notify(NULL, attr, &debug_info, sizeof(debug_info));
        k_msleep(10);
    }
}

static ssize_t debug_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset) {
    if (!device_is_ready(icm20948_device)) {
        return -EIO;
    }
    sensor_sample_fetch(icm20948_device);
    memcpy(&debug_info.data, icm20948_device->data,
           sizeof(struct icm20948_data));
    debug_info.time = k_uptime_get();
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &debug_info,
                             sizeof(debug_info));
}

static void subscribe_attr_cb(const struct bt_gatt_attr *attr, uint16_t value) {
    if (value & BT_GATT_CCC_NOTIFY) {
        LOG_INF("Debug service subscribed to");
        k_thread_resume(dbg_notify_tid);
    } else {
        LOG_INF("Debug service unsubscribed to");
        k_thread_suspend(dbg_notify_tid);
    }
}