cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(NuclearBriefcase C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(NuclearBriefcase main.c)

pico_set_program_name(NuclearBriefcase "NuclearBriefcase")
pico_set_program_version(NuclearBriefcase "0.1")

# Generate PIO header
pico_generate_pio_header(NuclearBriefcase ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(NuclearBriefcase 0)
pico_enable_stdio_usb(NuclearBriefcase 1)

# Add the standard library to the build
target_link_libraries(NuclearBriefcase
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(NuclearBriefcase PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(NuclearBriefcase
        hardware_i2c
        hardware_pio
)

pico_add_extra_outputs(NuclearBriefcase)

add_custom_command(
		TARGET NuclearBriefcase
		POST_BUILD
		COMMAND /usr/bin/picotool load -f NuclearBriefcase.uf2
)
