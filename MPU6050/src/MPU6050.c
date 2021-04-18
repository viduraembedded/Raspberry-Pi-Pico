#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "MPU6050.h"

void MPU6050_Reset()
{
    uint8_t reg[] = {0x6B, 0x00};
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, reg, sizeof(reg), false );
}
void MPU6050_ReadData(int16_t accelerometer[3], int16_t gyro[3], int16_t *temp)
{
    uint8_t buffer[6];
    // reading the accelerometer data
    uint8_t reg = 0x3B;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, &reg, sizeof(reg), false );
    i2c_read_blocking(I2C_PORT, MPU6050_ADDRESS, buffer, sizeof(buffer), false );

    accelerometer[0] = (buffer[0] << 8) | buffer[1];
    accelerometer[1] = (buffer[2] << 8) | buffer[3];
    accelerometer[2] = (buffer[4] << 8) | buffer[5];

    //Gyro data    
   reg = 0x43;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, &reg, sizeof(reg), false );
    i2c_read_blocking(I2C_PORT, MPU6050_ADDRESS, buffer, sizeof(buffer), false );

    gyro[0] = (buffer[0] << 8) | buffer[1];
    gyro[1] = (buffer[2] << 8) | buffer[3];
    gyro[2] = (buffer[4] << 8) | buffer[5];

    // read the temperture data
    reg = 0x41;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, &reg, sizeof(reg), false );
    i2c_read_blocking(I2C_PORT, MPU6050_ADDRESS, buffer, sizeof(buffer), false );
    
    *temp = (buffer[0] << 8) | buffer[1];
    
}