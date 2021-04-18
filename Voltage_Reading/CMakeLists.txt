#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(Voltage_Reading)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(Voltage_Reading
    Voltage_Reading.c
)
pico_enable_stdio_usb(Voltage_Reading 1)
pico_enable_stdio_uart(Voltage_Reading 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(Voltage_Reading)

#7. add target libraries
target_link_libraries(Voltage_Reading pico_stdlib hardware_adc)