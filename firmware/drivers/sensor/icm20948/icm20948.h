#ifndef DRIVERS_SENSOR_ICM20948_H_
#define DRIVERS_SENSOR_ICM20948_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

#define ICM20948_DEFAULT_ADDRESS 0xEA

struct icm20948_data {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

    int16_t magnet_x;
    int16_t magnet_y;
    int16_t magnet_z;

    int16_t temp;
};

struct icm20948_config {
    struct spi_dt_spec spi;
};

#endif