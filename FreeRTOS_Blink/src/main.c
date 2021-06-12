#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "FreeRTOS.h"
#include "task.h"

//Define Macros here
#define RED_LED_PIN 16
#define YELLOW_LED_PIN 17

//Define function prototyoes here
void vRedLEDTask( void * pvParameters );
void vYellowLEDTask( void * pvParameters );

// Main function implementation
int main() {

stdio_init_all();

gpio_init(RED_LED_PIN);
gpio_init(YELLOW_LED_PIN);
gpio_set_dir(RED_LED_PIN, GPIO_OUT);
gpio_set_dir(YELLOW_LED_PIN, GPIO_OUT);


BaseType_t xReturned;

TaskHandle_t xRedLEDHandle = NULL;
TaskHandle_t xYellowLEDHandle = NULL;

/* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    vRedLEDTask,       /* Function that implements the task. */
                    "RED LED Task",   /* Text name for the task. */
                    512,             /* Stack size in words, not bytes. */
                    NULL,            /* Parameter passed into the task. */
                    tskIDLE_PRIORITY + 1,/* Priority at which the task is created. */
                    &xRedLEDHandle );   

    xReturned = xTaskCreate(
                    vYellowLEDTask,       /* Function that implements the task. */
                    "Yellow LED Task",   /* Text name for the task. */
                    512,             /* Stack size in words, not bytes. */
                    NULL,            /* Parameter passed into the task. */
                    tskIDLE_PRIORITY + 1,/* Priority at which the task is created. */
                    &xYellowLEDHandle ); 

                    
    vTaskStartScheduler();
    
    while(1)
    {
        //configASSERT(0);    /* We should never get here */
    }

}
/*********************************************************
*Task Implementation 
**********************************************************
*/

// Red LED task implementation /functionality 
void vRedLEDTask( void * pvParameters )
{
    (void) pvParameters;

    for( ;; )
    {
        gpio_put(RED_LED_PIN, 1);
        vTaskDelay(500);
        gpio_put(RED_LED_PIN, 0);
        vTaskDelay(500);
    }
}

// Yellow LED task implementation/functionality 
void vYellowLEDTask( void * pvParameters )
{

    (void) pvParameters;

    for( ;; )
    {
        gpio_put(YELLOW_LED_PIN, 1);
        vTaskDelay(1000);
        gpio_put(YELLOW_LED_PIN, 0);
        vTaskDelay(1000);
    }
}