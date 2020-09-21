#include <unistd.h>

struct identifier {
    /* Variable to hold device address */
    uint8_t dev_addr;

    /* Variable that contains file descriptor */
    int8_t fd;
};

int8_t i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

void delay_us(uint32_t period, void *intf_ptr);

int8_t i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

void print_sensor_data(struct bme280_data *compilete_data);

int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev);
