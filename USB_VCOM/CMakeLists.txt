#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(USB_VCOM)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(USB_VCOM
    USB_VCOM.c
)
pico_enable_stdio_usb(USB_VCOM 1)
pico_enable_stdio_uart(USB_VCOM 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(USB_VCOM)

#7. add target libraries
target_link_libraries(USB_VCOM pico_stdlib)