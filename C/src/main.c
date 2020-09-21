#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <bme280.h>
#include <i2c_bme.h>


int main() {
    struct bme280_dev dev;
    struct identifier id;

    int8_t result = BME280_OK;

    if ((id.fd = open("/dev/i2c-1", O_RDWR)) < 0) {
	fprintf(stderr, "Failed to open the i2c bus %s\n", "/dev/i2c-1");
	exit(1);
}
    if(ioctl(id.fd, I2C_SLAVE, BME280_I2C_ADDR_PRIM) < 0) {
        fprintf(stderr, "Failed to acquire bus access\n");
        exit(1);
    }

    id.dev_addr = BME280_I2C_ADDR_PRIM;

    dev.intf = BME280_I2C_INTF;
    dev.read = i2c_read;
    dev.write = i2c_write;
    dev.delay_us = delay_us;

    dev.intf_ptr = &id;

    result = bme280_init(&dev);
    if(result != BME280_OK) {
        fprintf(stderr, "Failed to initialize the device (code %+d).\n", result);
        exit(1);
    }

    result = stream_sensor_data_forced_mode(&dev);
    if (result != BME280_OK) {
        fprintf(stderr, "Failed to stream sensor data (code %+d).\n", result);
        exit(1);
    }

    return 0;
}
