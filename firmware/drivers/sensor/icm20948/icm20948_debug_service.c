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

// Struct for storing sensor data and timestamp
static struct debug_data {
    int64_t time;
    struct icm20948_data data;
} debug_info;

// Mutex for accessing sensor data struct
K_MUTEX_DEFINE(sensor_data_mutex);

// Sensor device
static const struct device *icm20948_device =
    DEVICE_DT_GET_ONE(invensense_icm20948);

// Declare functions so they are available for thread and service definition
static void subscribe_attr_cb(const struct bt_gatt_attr *attr, uint16_t value);
static ssize_t debug_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset);
static void notify_sensor_value(struct k_work *work);

// Create workthread and sensor notification worker
K_THREAD_STACK_DEFINE(debug_workthread_area, 1024);
K_WORK_DEFINE(notify_work, notify_sensor_value);
static struct k_work_q work_q;

// Define timer handler and timer
void notify_timer_handler(struct k_timer *timer) {
    ARG_UNUSED(timer);
    k_work_submit(&notify_work);
}
K_TIMER_DEFINE(notify_timer, notify_timer_handler, NULL);

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

static void notify_sensor_value(struct k_work *work) {
    ARG_UNUSED(work);

    // Get attribute
    struct bt_gatt_attr *attr = bt_gatt_find_by_uuid(
        debug_service.attrs, debug_service.attr_count, &debug_info_uuid.uuid);

    // Make sure sensor device is initialized
    if (!device_is_ready(icm20948_device)) {
        LOG_ERR("ICM20948 device is not ready, stopping notifications");
        // Stop the notification timer if sensor is not online	
        k_timer_stop(&notify_timer);
        return;
    }

    // Wait for sensor to be available for reading and lock mutex
    int err = k_mutex_lock(&sensor_data_mutex, K_MSEC(10));
    if (err) {
        LOG_ERR("Error %d: Could not access sensor data mutex", err);
        return;
    }

    // Read and store data
    sensor_sample_fetch(icm20948_device);
    memcpy(&debug_info.data, icm20948_device->data,
           sizeof(struct icm20948_data));
    debug_info.time = k_uptime_get();

    // Notify with new sensor data
    bt_gatt_notify(NULL, attr, &debug_info, sizeof(debug_info));

    // Unlock mutex again
    k_mutex_unlock(&sensor_data_mutex);
}

static ssize_t debug_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset) {
    int err;

    // Make sure sensor device is initialized
    if (!device_is_ready(icm20948_device)) {
        return -EIO;
    }

    // Wait for sensor to be available for reading and lock mutex
    err = k_mutex_lock(&sensor_data_mutex, K_MSEC(10));
    if (err) {
        LOG_ERR("Error %d: Could not access sensor data mutex", err);
        return err;
    }

    // Read and store data
    sensor_sample_fetch(icm20948_device);
    memcpy(&debug_info.data, icm20948_device->data,
           sizeof(struct icm20948_data));
    debug_info.time = k_uptime_get();

    // Send sensor data to the reader
    err = bt_gatt_attr_read(conn, attr, buf, len, offset, &debug_info,
                            sizeof(debug_info));

    // Unlock mutex again
    k_mutex_unlock(&sensor_data_mutex);
    return err;
}

static void subscribe_attr_cb(const struct bt_gatt_attr *attr, uint16_t value) {
    if (value & BT_GATT_CCC_NOTIFY) {
        LOG_INF("Debug service subscribed to");

        // Init and start workqueue
        k_work_queue_init(&work_q);
        k_work_queue_start(&work_q, debug_workthread_area,
                           K_THREAD_STACK_SIZEOF(debug_workthread_area), 5,
                           NULL);

        // Start timer for sending sensor data notification every 20 msec
        k_timer_start(&notify_timer, K_NO_WAIT, K_MSEC(20));
    } else {
        LOG_INF("Debug service unsubscribed to");

        // Stop timer that sends sensor data notifications
        k_timer_stop(&notify_timer);
    }
}