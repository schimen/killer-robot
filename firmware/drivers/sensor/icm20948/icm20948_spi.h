
#ifndef DRIVERS_SENSOR_ICM20948_SPI_H_
#define DRIVERS_SENSOR_ICM20948_SPI_H_

#include <zephyr/drivers/spi.h>
#include <zephyr/types.h>

/**
 * @brief SPI transmit/receive wrapper function used to interface with ICM20948.
 *
 * @param bus SPI bus
 * @param tx_buffer Pointer to data buffer for SPI transmission.
 * @param rx_buffer Pointer to data buffer for SPI reception.
 * @param len Length of buffer
 * @return Value from spi_transceive_dt()
 */
int icm20948_spi_transceive(const struct spi_dt_spec *bus, uint8_t *tx_buffer,
                            uint8_t *rx_buffer, uint8_t len);

/**
 * @brief Write to register on ICM20948 via SPI
 *
 * @param bus SPI bus
 * @param bank The bank where the register is located
 * @param reg Address to the register
 * @param data Data that will be written to the register
 * @return Value from icm20948_spi_transceive()
 */
int icm20948_write_register(const struct spi_dt_spec *bus, uint8_t bank,
                            uint8_t reg, uint8_t data);

/**
 * @brief Read from register on ICM20948 via SPI
 *
 * @param bus SPI bus
 * @param bank The bank where the register is located
 * @param reg Address to the register
 * @param data Pointer to the byte where the result should be written
 * @return Value from icm20948_spi_transceive()
 */
int icm20948_read_register(const struct spi_dt_spec *bus, uint8_t bank,
                           uint8_t reg, uint8_t *data);

/**
 * @brief Set the user bank on ICM20948, and save it to a static variable
 *
 * @param bus SPI bus
 * @param bank The desired bank
 */
void icm20948_set_correct_bank(const struct spi_dt_spec *bus, uint8_t bank);

/**
 * @brief Write to register on AK09916 via SPI
 *
 * @param bus SPI bus
 * @param reg Address to the register
 * @param data Data that will be written to the register
 * @return Value from icm20948_spi_transceive()
 */
int ak09916_write_register(const struct spi_dt_spec *bus, uint8_t reg, uint8_t data);

/**
 * @brief Read from register on AK09916 via SPI 
 *
 * @param bus SPI bus
 * @param reg Address to the register
 * @param data Pointer to the byte where the result should be written
 * @return Value from icm20948_spi_transceive()
 */
int ak09916_read_register(const struct spi_dt_spec *bus, uint8_t reg, uint8_t *data);

#endif