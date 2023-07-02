
#include "icm20948_spi.h"
#include "icm20948_reg.h"

// SPI device
static struct spi_dt_spec spi_bus =
    SPI_DT_SPEC_GET(DT_NODELABEL(icm20948),
                    SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA |
                        SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
                    0U);

static int icm20948_spi_transceive(uint8_t *buffer, uint8_t len) {
    struct spi_buf txrx_buf = {
        .buf = buffer,
        .len = len,
    };
    const struct spi_buf_set txrx = {
        .buffers = &txrx_buf,
        .count = 1,
    };
    return spi_transceive_dt(&spi_bus, &txrx, &txrx);
}

void icm20948_set_correct_bank(uint8_t bank) {
    // Save the previously set user bank
    static uint8_t current_bank = 255;
    // Return if wanted user bank is the same as previously set
    if (bank == current_bank) {
        return;
    }
    uint8_t tx_buffer[2] = {REG_BANK_SEL, 0};
    tx_buffer[1] = bank;
    icm20948_spi_transceive(tx_buffer, 2);
    current_bank = bank;
}

int icm20948_write_register(uint8_t bank, uint8_t reg, uint8_t data) {
    uint8_t tx_buffer[2] = {0};
    tx_buffer[0] = reg;
    tx_buffer[1] = data;
    icm20948_set_correct_bank(bank);
    return icm20948_spi_transceive(tx_buffer, 2);
}

int icm20948_read_register(uint8_t bank, uint8_t reg, uint8_t *data) {
    int result;
    uint8_t buffer[2] = {0};
    buffer[0] = 0x80 | reg;
    icm20948_set_correct_bank(bank);
    result = icm20948_spi_transceive(buffer, 2);
    *data = buffer[1];
    return result;
}