#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "MPU6050.h"


#define SCL 19
#define SDA 18

int main()
{

    //intialize stdio
    stdio_init_all();
    //intialize i2c1
    i2c_init(I2C_PORT, 100*1000);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_set_function(SDA, GPIO_FUNC_I2C);

    // need to enable the pullups
    gpio_pull_up(SCL);
    gpio_pull_up(SDA);

    int16_t accelerometer[3], gyro[3], temp;
    MPU6050_Reset();
    while(1)
    {
        MPU6050_ReadData(accelerometer, gyro, &temp);
        printf("Accelerometer   X_OUT= %d   Y_OUT= %d   Z_OUT= %d\r\n",accelerometer[0], accelerometer[1], accelerometer[2] );
        printf("Gyro   X_OUT= %d   Y_OUT= %d   Z_OUT= %d\r\n",gyro[0], gyro[1], gyro[2] );
        printf("Temperature= %2f \r\n",(temp/340.0) + 36.53 );
        sleep_ms(500);
    }

    return 0;
}