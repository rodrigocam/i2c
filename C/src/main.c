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

#define I2C_FILE "/dev/i2c-1"

int main() {
    struct bme280_dev device;
    
    struct identifier id;
    id.dev_addr = BME280_I2C_ADDR_PRIM;

    int8_t result = BME280_OK;

    if ((id.fd = open(I2C_FILE, O_RDWR)) < 0) {
	fprintf(stderr, "Failed to open the i2c bus %s\n", I2C_FILE);
	exit(1);
    }
    
    if(ioctl(id.fd, I2C_SLAVE, id.dev_addr) < 0) {
        fprintf(stderr, "Failed to acquire bus access\n");
        exit(1);
    }

    device.intf = BME280_I2C_INTF;
    device.read = i2c_read;
    device.write = i2c_write;
    device.delay_us = delay_us;
    device.intf_ptr = &id;

    result = bme280_init(&device);
    
    if(result != BME280_OK) {
        fprintf(stderr, "Failed to initialize the device (code %+d).\n", result);
        exit(1);
    }

    result = stream_sensor_data_forced_mode(&device);

    if (result != BME280_OK) {
        fprintf(stderr, "Failed to stream sensor data (code %+d).\n", result);
        exit(1);
    }

    return 0;
}
