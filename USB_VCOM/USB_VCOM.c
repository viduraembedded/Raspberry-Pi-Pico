#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"


int main()
{

    //intialize stdio
    stdio_init_all();
    while(1)
    {
        printf("Hello World\r\n");
        sleep_ms(1000);
    }
    return 0;
}