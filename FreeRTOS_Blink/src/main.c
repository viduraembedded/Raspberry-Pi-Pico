#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "FreeRTOS.h"
#include "task.h"

void vGreenLEDTask( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    */
    for( ;; )
    {
        gpio_put(25, 1);
        vTaskDelay(1000);
        gpio_put(25, 0);
        vTaskDelay(1000);
    }
}

int main() {

stdio_init_all();

gpio_init(25);
gpio_set_dir(25, GPIO_OUT);

BaseType_t xReturned;
TaskHandle_t xHandle = NULL;
/* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    vGreenLEDTask,       /* Function that implements the task. */
                    "Blinky Task",   /* Text name for the task. */
                    512,             /* Stack size in words, not bytes. */
                    NULL,            /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle );   

    vTaskStartScheduler();
    
    while(1)
    {
        //configASSERT(0);    /* We should never get here */
    }

}