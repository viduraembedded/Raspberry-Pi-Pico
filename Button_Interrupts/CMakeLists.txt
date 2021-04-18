#1. specify the CMake minimum version
cmake_minimum_required(VERSION 3.19)

#2. include the pico sdk here
include(pico_sdk_import.cmake)

#3. Specify the project name
project(Button_Interrupts)

#4. initialize the pico sdk
pico_sdk_init()

#5. add executable
add_executable(Button_Interrupts
    Button_Interrupts.c
)

#6. Add extra outputs to the blink project
pico_add_extra_outputs(Button_Interrupts)

#7. add target libraries
target_link_libraries(Button_Interrupts pico_stdlib)