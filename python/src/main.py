#!/usr/bin/python

import smbus2
import bme280

import RPi_I2C_driver
import time

I2C_PORT = 1
DEVICE_ADDR = 0x76

BUS = smbus2.SMBus(I2C_PORT)
CALIBRATION_PARAMS = bme280.load_calibration_params(BUS, DEVICE_ADDR)

LCD = RPi_I2C_driver.lcd()
LCD.lcd_clear()

while True:
    DATA = bme280.sample(BUS, DEVICE_ADDR, CALIBRATION_PARAMS)
    LCD.lcd_display_string(f"T. {DATA.temperature:.2f}, H. {DATA.humidity:.2f}, P. {DATA.pressure:.2f}", 1) 
    print(f"printed to LCD {DATA.temperature}, {DATA.humidity}, {DATA.pressure}")
    LCD.lcd_clear()
    time.sleep(1)
