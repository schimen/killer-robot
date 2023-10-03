#ifndef DRIVERS_SENSOR_ICM20948_REG_H_
#define DRIVERS_SENSOR_ICM20948_REG_H_

#define ICM20948_DEFAULT_ADDRESS 0xEA
#define AK09916_DEFAULT_ADDRESS  0x0C
#define AK09916_DEFAULT_ID       0x09
#define REG_BANK_SEL             0x7F

/* BANK 0 */
#define REG_WHO_AM_I             0x00
#define REG_USER_CTRL            0x03
#define REG_PWR_MGMT_1           0x06
#define REG_ACCEL_XOUT_H         0x2D
#define REG_ACCEL_XOUT_L         0x2E
#define REG_ACCEL_YOUT_H         0x2F
#define REG_ACCEL_YOUT_L         0x30
#define REG_ACCEL_ZOUT_H         0x31
#define REG_ACCEL_ZOUT_L         0x32
#define REG_GYRO_XOUT_H          0x33
#define REG_GYRO_XOUT_L          0x34
#define REG_GYRO_YOUT_H          0x35
#define REG_GYRO_YOUT_L          0x36
#define REG_GYRO_ZOUT_H          0x37
#define REG_GYRO_ZOUT_L          0x38
#define REG_TEMP_OUT_H           0x39
#define REG_TEMP_OUT_L           0x3A
#define REG_EXT_SLV_SENS_DATA_00 0x3B

/* BANK 2 */
#define REG_GYRO_SMPLRT_DIV    0x00
#define REG_GYRO_CONFIG_1      0x01
#define REG_GYRO_CONFIG_2      0x02
#define REG_ODR_ALIGN_EN       0x09
#define REG_ACCEL_SMPLRT_DIV_1 0x10
#define REG_ACCEL_SMPLRT_DIV_2 0x11
#define REG_ACCEL_CONFIG       0x14
#define REG_ACCEL_CONFIG_2     0x15

/* BANK 3 */
#define REG_I2C_MST_CTRL       0x01
#define REG_I2C_MST_DELAY_CTRL 0x02
#define REG_I2C_SLV0_ADDR      0x03
#define REG_I2C_SLV0_REG       0x04
#define REG_I2C_SLV0_CTRL      0x05
#define REG_I2C_SLV0_DO        0x06

/* AK09916 registers */
#define REG_WIA1  0x00
#define REG_WIA2  0x01
#define REG_HXL   0x11
#define REG_HXH   0x12
#define REG_HYL   0x13
#define REG_HYH   0x14
#define REG_HZL   0x15
#define REG_HZH   0x16
#define REG_CNTL2 0x31
#define REG_CNTL3 0x32

#endif