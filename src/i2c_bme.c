#include <bme280.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#include <i2c_bme.h>

int8_t i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
        struct identifier id;

            id = *((struct identifier *)intf_ptr);

                write(id.fd, &reg_addr, 1);
                    read(id.fd, data, len);

                        return 0;
                        }

int8_t i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
        uint8_t *buf;
            struct identifier id;

                id = *((struct identifier *)intf_ptr);

                    buf = malloc(len + 1);
                        buf[0] = reg_addr;
                            memcpy(buf + 1, data, len);
                                if (write(id.fd, buf, len + 1) < (uint16_t)len)
                                        {
                                                    return BME280_E_COMM_FAIL;
                                                        }

                                    free(buf);

                                        return BME280_OK;
                                        }

/*!
 * @brief This function provides the delay for required time (Microseconds) as per the input provided in some of the
 * APIs
 */
void delay_us(uint32_t period, void *intf_ptr)
{
    usleep(period);
}

void print_sensor_data(struct bme280_data *comp_data) {
    float temp, press, hum;
    
    temp = comp_data->temperature;
    press = 0.01 * comp_data->pressure;
    hum = comp_data->humidity;
    printf("%0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", temp, press, hum);
}

void write_csv(float temperature, float pressure, float humidity) {
    time_t t = time(NULL);
    struct tm ts = *localtime(&t);

    FILE *f = fopen("measures.csv", "a");

    fprintf(f, "%f, %f, %f, %02d/%02d/%02d:%02d:%02d:%02d\n", temperature, pressure, humidity, ts.tm_mday, ts.tm_mon + 1, ts.tm_year + 1900, ts.tm_hour, ts.tm_min, ts.tm_sec);
    
    fclose(f);
}

int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev) {
    /* Variable to define the result */
    int8_t rslt = BME280_OK;

    /* Variable to define the selecting sensors */
    uint8_t settings_sel = 0;

    /* Variable to store minimum wait time between consecutive measurement in force mode */
    uint32_t req_delay;

    /* Structure to get the pressure, temperature and humidity values */
    struct bme280_data comp_data;

    /* Recommended mode of operation: Indoor navigation */
    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t = BME280_OVERSAMPLING_2X;
    dev->settings.filter = BME280_FILTER_COEFF_16;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
    rslt = bme280_set_sensor_settings(settings_sel, dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor settings (code %+d).", rslt);

        return rslt;
    }

    /*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
     *  and the oversampling configuration. */
    req_delay = bme280_cal_meas_delay(&dev->settings);

    int count = 0;
    double temperature_acc, pressure_acc, humidity_acc;

    /* Continuously stream sensor data */
    while (1)
    {
        count +=1;
        sleep(1);
        /* Set the sensor to forced mode */
        rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
        if (rslt != BME280_OK)
        {
            fprintf(stderr, "Failed to set sensor mode (code %+d).", rslt);
            break;
        }

        /* Wait for the measurement to complete and print data */
        dev->delay_us(req_delay, dev->intf_ptr);
        rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
        if (rslt != BME280_OK)
        {
            fprintf(stderr, "Failed to get sensor data (code %+d).", rslt);
            break;
        }
        
        temperature_acc += comp_data.temperature;
        pressure_acc += comp_data.pressure;
        humidity_acc += comp_data.humidity;
        
        if(count == 10) {
            printf("Registrando novas medidas!\n");
            write_csv(temperature_acc / 10, pressure_acc / 10, humidity_acc / 10);
            temperature_acc = 0;
            pressure_acc = 0;
            humidity_acc = 0;
            count = 0;
        }
    }

    return rslt;
}

