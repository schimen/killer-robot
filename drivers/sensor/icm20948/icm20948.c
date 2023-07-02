#define DT_DRV_COMPAT invensense_icm20948

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

// #include "icm20948.h"
#include "icm20948_reg.h"
#include "icm20948_spi.h"

LOG_MODULE_REGISTER(ICM20948);

#define ICM20948_DEFAULT_ADDRESS 0xEA

struct icm20948_data {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

    int16_t temp;
};

struct icm420948_config {
    struct spi_dt_spec spi;
};

/*
    Conversion functions are not yet tested and probably wrong, fix these later
*/
#define SENSOR_G 2
#define SENSOR_DPS 250
static void icm20948_convert_accel(struct sensor_value *val, int16_t raw_val) {
    const int16_t sensitivity = ((2 ^ 16) / 2) / SENSOR_G;
    val->val1 = raw_val / sensitivity;
    val->val2 = raw_val % sensitivity;
}

static void icm20948_convert_gyro(struct sensor_value *val, int16_t raw_val) {
    const int16_t sensitivity = ((2 ^ 16) / 2) / SENSOR_DPS;
    val->val1 = raw_val / sensitivity;
    val->val2 = raw_val % sensitivity;
}

static void icm20948_convert_temp(struct sensor_value *val, int16_t raw_val) {
    val->val1 = (raw_val / 334) + 21;
    val->val2 = raw_val % 334;
}

/**
 * @brief Channel get function for ICM20948.
 * For information, see documentation for sensor_channel_get() function.
 */
static int icm20948_channel_get(const struct device *dev,
                                enum sensor_channel chan,
                                struct sensor_value *val) {
    struct icm20948_data *data = dev->data;

    switch (chan) {
    case SENSOR_CHAN_ACCEL_XYZ:
        icm20948_convert_accel(&val[0], data->accel_x);
        icm20948_convert_accel(&val[1], data->accel_y);
        icm20948_convert_accel(&val[2], data->accel_z);
        break;
    case SENSOR_CHAN_ACCEL_X:
        icm_20948_convert_accel(val, data->accel_x);
        break;
    case SENSOR_CHAN_ACCEL_Y:
        icm20948_convert_accel(val, data->accel_y);
        break;
    case SENSOR_CHAN_ACCEL_Z:
        icm20948_convert_accel(val, data->accel_z);
        break;
    case SENSOR_CHAN_GYRO_XYZ:
        icm20948_convert_gyro(&val[0], data->gyro_x);
        icm20948_convert_gyro(&val[1], data->gyro_y);
        icm20948_convert_gyro(&val[2], data->gyro_z);
        break;
    case SENSOR_CHAN_GYRO_X:
        icm20948_convert_gyro(val, data->gyro_x);
        break;
    case SENSOR_CHAN_GYRO_Y:
        icm20948_convert_gyro(val, data->gyro_y);
        break;
    case SENSOR_CHAN_GYRO_Z:
        icm20948_convert_gyro(val, data->gyro_z);
        break;
    case SENSOR_CHAN_DIE_TEMP:
        icm20948_convert_temp(val, data->temp);
        break;
    default:
        return -ENOTSUP;
    }

    return 0;
}

/**
 * @brief Sample fetch function for ICM20948.
 * For information see, see documentation for sensor_sample_fetch() function.
 */
static int icm20948_sample_fetch(const struct device *dev,
                                 enum sensor_channel chan) {
    struct icm20948_data *data = dev->data;
    int err;
    uint8_t buffer[15];

    // Set address to start reading from
    buffer[0] = REG_ACCEL_XOUT_H | 0x80;
    icm20948_set_correct_bank(0);
    err = icm20948_spi_transceive((uint8_t *)buffer, 14);
    if (err) {
        return err;
    }

    int16_t *received_data = (int16_t *)&buffer[1];

    // NB!!! Sjekk om denne dataen stemmer
    data->accel_x = sys_be16_to_cpu(received_data[0]);
    data->accel_y = sys_be16_to_cpu(received_data[1]);
    data->accel_z = sys_be16_to_cpu(received_data[2]);
    data->gyro_x = sys_be16_to_cpu(received_data[3]);
    data->gyro_y = sys_be16_to_cpu(received_data[4]);
    data->gyro_z = sys_be16_to_cpu(received_data[5]);
    data->temp = sys_be16_to_cpu(received_data[6]);
    return 0;
}

int icm20948_init() {
    // Reset unit with best avialble clock
    icm20948_write_register(0, REG_PWR_MGMT_1, 0xC1);
    // Make sure chip is reset
    k_msleep(100);
    // Exit sleep mode
    icm20948_write_register(0, REG_PWR_MGMT_1, 0x01);
    // Enable ODR start-time alignment
    icm20948_write_register(2, REG_ODR_ALIGN_EN, 0x01);
    // Gyro Config
    icm20948_write_register(2, REG_GYRO_SMPLRT_DIV, 0x00);
    icm20948_write_register(2, REG_GYRO_CONFIG_1, 0x01);
    // Accel Config
    icm20948_write_register(2, REG_ACCEL_SMPLRT_DIV_1, 0x00);
    icm20948_write_register(2, REG_ACCEL_SMPLRT_DIV_2, 0x00);
    icm20948_write_register(2, REG_ACCEL_CONFIG, 0x01);
    // Set serial interface to SPI only
    uint8_t value;
    icm20948_read_register(0, REG_USER_CTRL, &value);
    value |= 0x10;
    icm20948_write_register(0, REG_USER_CTRL, value);
    // Check that address is correct
    icm20948_read_register(0, REG_WHO_AM_I, &value);
    if (value != ICM20948_DEFAULT_ADDRESS) {
        LOG_ERR("Error: Unexpected address at sensor. Expected 0x%02X, "
                "received 0x%02X",
                ICM20948_DEFAULT_ADDRESS, value);
        return -1;
    }
    LOG_INF("Sensor address: 0x%02X", value);
    return 0;
}

void icm20948_read_accelerometer(struct icm20948_data *data) {
    uint8_t buffer[7];
    // Start reading from first relevant address
    buffer[0] = REG_ACCEL_XOUT_H | 0x80;
    icm20948_spi_transceive(buffer, 7);
    data->accel_x = ((int16_t)buffer[1] << 8) + buffer[2];
    data->accel_y = ((int16_t)buffer[3] << 8) + buffer[4];
    data->accel_z = ((int16_t)buffer[5] << 8) + buffer[6];
}

void icm20948_read_gyroscope(struct icm20948_data *data) {
    uint8_t buffer[7];
    // Start reading from first relevant address
    buffer[0] = REG_GYRO_XOUT_H | 0x80;
    icm20948_spi_transceive(buffer, 7);
    data->gyro_x = ((int16_t)buffer[1] << 8) + buffer[2];
    data->gyro_y = ((int16_t)buffer[3] << 8) + buffer[4];
    data->gyro_z = ((int16_t)buffer[5] << 8) + buffer[6];
}

void icm20948_read_temperature(struct icm20948_data *data) {
    uint8_t buffer[3];
    // Start reading from first relevant address
    buffer[0] = REG_TEMP_OUT_H | 0x80;
    icm20948_spi_transceive(buffer, 3);
    data->temp = ((uint16_t)buffer[1] << 8) + buffer[2];
}

static const struct sensor_driver_api icm20948_driver_api = {
    .sample_fetch = icm20948_sample_fetch,
    .channel_get = icm20948_channel_get,
};

#define ICM20948_SPI_CONFIG                                                    \
    SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) |     \
        SPI_TRANSFER_MSB

#define ICM20948_DEFINE(inst)                                                  \
    static struct icm20948_data icm20948_data_##inst;                          \
    static const struct icm20948_config icm20948_config_##inst = {             \
        .spi = SPI_DT_SPEC_INST_GET(inst, ICM20948_SPI_CONFIG, 0U);            \
    };                                                                         \
    SENSOR_DEVICE_DT_INST_DEFINE(                                              \
        inst, icm_20948_init, NULL, &icm20948_data_##inst,                     \
        &icm20948_config_##inst, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,    \
        &icm20948_driver_api);


#if DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0
#error"ICM20948 not defined in DTS"
#endif

DT_INST_FOREACH_STATUS_OKAY(ICM20948_DEFINE)
