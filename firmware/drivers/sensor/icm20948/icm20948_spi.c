#include "icm20948_spi.h"
#include "icm20948_reg.h"

int icm20948_spi_transceive(const struct spi_dt_spec *bus, uint8_t *tx_buffer,
                            uint8_t *rx_buffer, uint8_t len) {
    struct spi_buf tx_buf = {
        .buf = tx_buffer,
        .len = len,
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1,
    };
    struct spi_buf rx_buf = {
        .buf = rx_buffer,
        .len = len,
    };
    const struct spi_buf_set rx = {
        .buffers = &rx_buf,
        .count = 1,
    };
    return spi_transceive_dt(bus, &tx, &rx);
}

void icm20948_set_correct_bank(const struct spi_dt_spec *bus, uint8_t bank) {
    // Save the previously set user bank
    static uint8_t current_bank = 255;
    // Return if wanted user bank is the same as previously set
    if (bank == current_bank) {
        return;
    }
    uint8_t tx_buffer[2] = {REG_BANK_SEL, bank << 4};
    uint8_t rx_buffer[2];
    icm20948_spi_transceive(bus, tx_buffer, rx_buffer, 2);
    current_bank = bank;
}

int icm20948_write_register(const struct spi_dt_spec *bus, uint8_t bank,
                            uint8_t reg, uint8_t data) {
    uint8_t tx_buffer[2] = {0};
    tx_buffer[0] = reg;
    tx_buffer[1] = data;
    uint8_t rx_buffer[2];
    icm20948_set_correct_bank(bus, bank);
    return icm20948_spi_transceive(bus, tx_buffer, rx_buffer, 2);
}

int icm20948_read_register(const struct spi_dt_spec *bus, uint8_t bank,
                           uint8_t reg, uint8_t *data) {
    int result;
    uint8_t tx_buffer[2] = {0};
    tx_buffer[0] = 0x80 | reg;
    uint8_t rx_buffer[2] = {0};
    icm20948_set_correct_bank(bus, bank);
    result = icm20948_spi_transceive(bus, tx_buffer, rx_buffer, 2);
    *data = rx_buffer[1];
    return result;
}

int ak09916_write_register(const struct spi_dt_spec *bus, uint8_t reg, uint8_t data) {
    int err = 0;
    // Set I2C slave 0 to write to AK09916 address
    err |= icm20948_write_register(bus, 3, REG_I2C_SLV0_ADDR, AK09916_DEFAULT_ADDRESS);
    // Start slave 0 write from register
    err |= icm20948_write_register(bus, 3, REG_I2C_SLV0_REG, reg);
    // Set data to be written
    err |= icm20948_write_register(bus, 3, REG_I2C_SLV0_DO, data);
    k_msleep(50);
    return err == 0 ? 0 : -1;
}

int ak09916_read_register(const struct spi_dt_spec *bus, uint8_t reg, uint8_t *data) {
    int err;
    // Set I2C slave 0 to read from AK09916 address
    icm20948_write_register(bus, 3, REG_I2C_SLV0_ADDR, 0x80 | AK09916_DEFAULT_ADDRESS);
    // Set register to start reading from
    icm20948_write_register(bus, 3, REG_I2C_SLV0_REG, reg);
    // Enable read data from slave 0 and read 1 byte
    icm20948_write_register(bus, 3, REG_I2C_SLV0_CTRL, 0x81);
    k_msleep(50);
    // Read data from I2C
    err = icm20948_read_register(bus, 0, REG_EXT_SLV_SENS_DATA_00, data);
    return err;
}