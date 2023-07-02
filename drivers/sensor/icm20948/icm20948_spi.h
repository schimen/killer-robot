
#ifndef DRIVERS_SENSOR_ICM20948_SPI_H_
#define DRIVERS_SENSOR_ICM20948_SPI_H_

#include <zephyr/types.h>

/**
 * @brief SPI transmit/receive wrapper function used to interface with ICM20948.
 * NB!!! This function uses the same buffer to read and write (the tx buffer is
 * overwrittern with result after each transmit). I don't know if this is a bad
 * idea and I should check that out.
 *
 * @param buffer Pointer to data buffer for SPI transmission. This will be used
 * for transmission and responses will overwrite previously transmitted bytes.
 * @param len Length of buffer
 * @return Value from spi_transceive_dt()
 */
int icm20948_spi_transceive(uint8_t *buffer, uint8_t len);

/**
 * @brief Write to register on ICM20948 via SPI
 * 
 * @param bank The bank where the register is located
 * @param reg Address to the register
 * @param data Data that will be written to the register
 * @return Value from icm20948_spi_transceive()
 */
int icm20948_write_register(uint8_t bank, uint8_t reg, uint8_t data);

/**
 * @brief Read from register on ICM20948 via SPI
 * 
 * @param bank The bank where the register is located
 * @param reg Address to the register
 * @param data Pointer to the byte where the result should be written
 * @return Value from icm20948_spi_transceive()
 */
int icm20948_read_register(uint8_t bank, uint8_t reg, uint8_t *data);

/**
 * @brief Set the user bank on ICM20948, and save it to a static variable
 * 
 * @param bank The desired bank
 */
void icm20948_set_correct_bank(uint8_t bank);

#endif