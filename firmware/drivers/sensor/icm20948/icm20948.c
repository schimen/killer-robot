#define DT_DRV_COMPAT invensense_icm20948

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/types.h>

#include "icm20948.h"
#include "icm20948_reg.h"
#include "icm20948_spi.h"

LOG_MODULE_REGISTER(ICM20948, CONFIG_SENSOR_LOG_LEVEL);

#define ICM20948_ACCEL_RANGE 2048 // At +-16G sensitivity
#define ICM20948_GYRO_RANGE 16    // At +-2000 dps sensitivity
#define ICM20948_TEMP_RANGE 334
#define AK09916_MAGNETOMETER_RANGE 7

/*
    Conversion functions used in this driver
    TODO: check and fix these so the conversion is correct
*/
static void icm20948_convert_accel(struct sensor_value *val, int16_t raw_val) {
    val->val1 = raw_val / ICM20948_ACCEL_RANGE;
    val->val2 = raw_val % ICM20948_ACCEL_RANGE;
}

static void icm20948_convert_gyro(struct sensor_value *val, int16_t raw_val) {
    val->val1 = raw_val / ICM20948_GYRO_RANGE;
    val->val2 = raw_val % ICM20948_GYRO_RANGE;
}

static void icm20948_convert_magn(struct sensor_value *val, int16_t raw_val) {
    val->val1 = raw_val / AK09916_MAGNETOMETER_RANGE;
    val->val2 = raw_val % AK09916_MAGNETOMETER_RANGE;
}

static void icm20948_convert_temp(struct sensor_value *val, int16_t raw_val) {
    val->val1 = ((raw_val - 21) / ICM20948_TEMP_RANGE) + 21;
    val->val2 = (raw_val - 21) % ICM20948_TEMP_RANGE;
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
        icm20948_convert_accel(val, data->accel_x);
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
    case SENSOR_CHAN_MAGN_XYZ:
        icm20948_convert_magn(&val[0], data->magn_x);
        icm20948_convert_magn(&val[1], data->magn_y);
        icm20948_convert_magn(&val[2], data->magn_z);
        break;
    case SENSOR_CHAN_MAGN_X:
        icm20948_convert_magn(val, data->magn_x);
        break;
    case SENSOR_CHAN_MAGN_Y:
        icm20948_convert_magn(val, data->magn_y);
        break;
    case SENSOR_CHAN_MAGN_Z:
        icm20948_convert_magn(val, data->magn_z);
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
    const struct icm20948_config *config = dev->config;
    const struct spi_dt_spec *bus = &config->spi;
    int err;
    uint8_t rx_buffer[23];
    uint8_t tx_buffer[sizeof(rx_buffer)] = {0};

    // Set address to start reading from
    tx_buffer[0] = REG_ACCEL_XOUT_H | 0x80;
    icm20948_set_correct_bank(bus, 0);
    err = icm20948_spi_transceive(bus, tx_buffer, rx_buffer, sizeof(rx_buffer));
    if (err) {
        LOG_ERR("ERROR: Failed to read data from sensor (%d)", err);
        return err;
    }

    int16_t *received_data = (int16_t *)&rx_buffer[1];
    data->accel_x = sys_be16_to_cpu(received_data[0]);
    data->accel_y = sys_be16_to_cpu(received_data[1]);
    data->accel_z = sys_be16_to_cpu(received_data[2]);
    data->gyro_x = sys_be16_to_cpu(received_data[3]);
    data->gyro_y = sys_be16_to_cpu(received_data[4]);
    data->gyro_z = sys_be16_to_cpu(received_data[5]);
    data->temp = sys_be16_to_cpu(received_data[6]);
    data->magn_x = sys_be16_to_cpu(received_data[7]);
    data->magn_y = sys_be16_to_cpu(received_data[8]);
    data->magn_z = sys_be16_to_cpu(received_data[9]);
    return 0;
}

static int icm20948_init(const struct device *dev) {
    // Init SPI
    const struct icm20948_config *config = dev->config;
    const struct spi_dt_spec *bus = &config->spi;
    uint8_t value;
    if (!spi_is_ready_dt(bus)) {
        LOG_ERR("SPI not ready");
        return -ENODEV;
    }

    // Reset unit with best available clock
    icm20948_write_register(bus, 0, REG_PWR_MGMT_1, 0x81);
    // Make sure chip is reset
    k_msleep(100);
    // Exit sleep mode
    icm20948_write_register(bus, 0, REG_PWR_MGMT_1, 0x01);

    // Check that we have contact by checking that address is correct
    icm20948_read_register(bus, 0, REG_WHO_AM_I, &value);
    if (value != ICM20948_DEFAULT_ADDRESS) {
        LOG_ERR("Error: Unexpected address at sensor. Expected 0x%02X, "
                "received 0x%02X",
                ICM20948_DEFAULT_ADDRESS, value);
        return -EIO;
    }
    // Enable ODR start-time alignment
    icm20948_write_register(bus, 2, REG_ODR_ALIGN_EN, 0x01);
    // Gyro Config
    icm20948_write_register(bus, 2, REG_GYRO_SMPLRT_DIV, 0x00);
    // Set gyro full scale to 2000dps and disable DLPF
    icm20948_write_register(bus, 2, REG_GYRO_CONFIG_1, 0x06);
    // Accel Config
    icm20948_write_register(bus, 2, REG_ACCEL_SMPLRT_DIV_1, 0x00);
    icm20948_write_register(bus, 2, REG_ACCEL_SMPLRT_DIV_2, 0x00);
    // Set accel full scale to 16g and disable DLPF
    icm20948_write_register(bus, 2, REG_ACCEL_CONFIG, 0x06);

    // Enable I2C master, disable FIFO and DMP
    icm20948_write_register(bus, 0, REG_USER_CTRL, 0x20);
    // Set I2C clock to 345.6 kHz, 46.67% duty cycle
    icm20948_write_register(bus, 3, REG_I2C_MST_CTRL, 0x07);
    // Reset AK09916
    ak09916_write_register(bus, REG_CNTL3, 0x01);
    k_msleep(100);
    // Set address and register to start reading from
    icm20948_write_register(bus, 3, REG_I2C_SLV0_ADDR, 0x80 | AK09916_DEFAULT_ADDRESS);
    icm20948_write_register(bus, 3, REG_I2C_SLV0_REG, 0x00);
    // Enable read data from slave 0 and read 1 byte
    icm20948_write_register(bus, 3, REG_I2C_SLV0_CTRL, 0x81);
    icm20948_read_register(bus, 0, REG_EXT_SLV_SENS_DATA_00, &value);
    // Check that default ID on AK09916 is correct
    ak09916_read_register(bus, REG_WIA2, &value);
    if (value != AK09916_DEFAULT_ID) {
        LOG_WRN("Error: Unexpected ID at magnetometer. Expected 0x%02X, "
                "received 0x%02X",
                AK09916_DEFAULT_ID, value);
    }
    // Set AK09916 to continous mode 100Hz
    ak09916_write_register(bus, REG_CNTL2, 0x08);
    // Set I2C slave 0 to write to AK09916 address
    icm20948_write_register(bus, 3, REG_I2C_SLV0_ADDR, 0x80 | AK09916_DEFAULT_ADDRESS);
    // Start slave 0 read from data register
    icm20948_write_register(bus, 3, REG_I2C_SLV0_REG, REG_HXL);
    // Enable read data from slave 0 and read 8 bytes every time
    icm20948_write_register(bus, 3, REG_I2C_SLV0_CTRL, 0x88);
    return 0;
}

static const struct sensor_driver_api icm20948_api = {
    .sample_fetch = &icm20948_sample_fetch,
    .channel_get = &icm20948_channel_get,
};

#define ICM20948_SPI_CONFIG                                                    \
    SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) |     \
        SPI_TRANSFER_MSB

#define ICM20948_DEFINE(inst)                                                  \
    static struct icm20948_data icm20948_data_##inst;                          \
    static const struct icm20948_config icm20948_config_##inst = {             \
        .spi = SPI_DT_SPEC_INST_GET(inst, ICM20948_SPI_CONFIG, 0U),            \
    };                                                                         \
    SENSOR_DEVICE_DT_INST_DEFINE(inst, icm20948_init, NULL,                    \
                                 &icm20948_data_##inst,                        \
                                 &icm20948_config_##inst, POST_KERNEL,         \
                                 CONFIG_SENSOR_INIT_PRIORITY, &icm20948_api);

DT_INST_FOREACH_STATUS_OKAY(ICM20948_DEFINE)