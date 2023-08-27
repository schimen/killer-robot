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
    uint8_t tx_buffer[2] = {REG_BANK_SEL, 0};
    uint8_t rx_buffer[2];
    tx_buffer[1] = bank;
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