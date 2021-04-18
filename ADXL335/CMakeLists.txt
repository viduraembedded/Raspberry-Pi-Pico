#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(ADXL335)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(ADXL335
ADXL335.c
)
pico_enable_stdio_usb(ADXL335 1)
pico_enable_stdio_uart(ADXL335 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(ADXL335)

#7. add target libraries
target_link_libraries(ADXL335 pico_stdlib hardware_gpio hardware_adc)