#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(1.8inch_TFT_Display)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(1.8inch_TFT_Display
    src/main.c
    src/st7735.c
    src/fonts.c
)

target_include_directories(1.8inch_TFT_Display PUBLIC inc/)

pico_enable_stdio_usb(1.8inch_TFT_Display 1)
pico_enable_stdio_uart(1.8inch_TFT_Display 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(1.8inch_TFT_Display)

#7. add target libraries
target_link_libraries(1.8inch_TFT_Display pico_stdlib hardware_gpio hardware_spi)