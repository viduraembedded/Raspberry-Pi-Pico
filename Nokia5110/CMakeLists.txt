#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(Nokia5110)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(Nokia5110
    src/main.c
    src/Nokia5110.c

)

target_include_directories(Nokia5110 PUBLIC inc/)

pico_enable_stdio_usb(Nokia5110 1)
pico_enable_stdio_uart(Nokia5110 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(Nokia5110)

#7. add target libraries
target_link_libraries(Nokia5110 pico_stdlib hardware_gpio hardware_spi)