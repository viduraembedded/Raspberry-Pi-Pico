cmake_minimum_required(VERSION 3.19)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
#add_subdirectory(pico-cpp)
project(FreeRTOS_Blink)

# initialize the Pico SDK
pico_sdk_init()


add_executable(FreeRTOS_Blink
    src/main.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(FreeRTOS_Blink pico_stdlib hardware_gpio hardware_clocks)
add_library(FreeRTOS STATIC 
FreeRTOS-Kernel/croutine.c
FreeRTOS-Kernel/event_groups.c
FreeRTOS-Kernel/list.c
FreeRTOS-Kernel/queue.c
FreeRTOS-Kernel/stream_buffer.c
FreeRTOS-Kernel/tasks.c
FreeRTOS-Kernel/timers.c FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c FreeRTOS-Kernel/portable/MemMang/heap_4.c)
target_include_directories(FreeRTOS PUBLIC FreeRTOS-Kernel/include inc/ FreeRTOS-Kernel/portable/GCC/ARM_CM0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(FreeRTOS_Blink)
# enable usb output, disable uart output
pico_enable_stdio_usb(FreeRTOS_Blink 1)
pico_enable_stdio_uart(FreeRTOS_Blink 0)

target_link_libraries(FreeRTOS_Blink FreeRTOS)

add_custom_command(TARGET FreeRTOS_Blink
        POST_BUILD
        COMMAND arm-none-eabi-size FreeRTOS_Blink.elf
        )
