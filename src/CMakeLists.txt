cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS ON)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(NuclearBriefcase C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(NuclearBriefcase main.c
		defines/config.h
		modules/numbers/numbers.c
		modules/numbers/numbers.h
		state.c
		state.h
		renderer.c
		renderer.h
		modules/status/status.c
		modules/status/status.h
		modules/wsleds/wsleds.c
		modules/wsleds/wsleds.h
		anim.c
		anim.h
		modules/wsleds/wsleds_data.h
		modules/piezo/piezo.c
		modules/piezo/piezo.h
		modules/piezo/piezo_data.c
		modules/piezo/piezo_data.h
)

pico_set_program_name(NuclearBriefcase "NuclearBriefcase")
pico_set_program_version(NuclearBriefcase "0.1")

get_target_property(PICO_STD_INCLUDE pico_stdlib INTERFACE_INCLUDE_DIRECTORIES)

target_compile_options(NuclearBriefcase PRIVATE
		$<$<CONFIG:Debug>:
		-Og # disable if you get <optimized out>
		-g3
		-Wall
		-Wextra
		-Winline
		#		-Werror
		-Wno-error=old-style-declaration
		-march=armv8-m.main
		-mtune=cortex-m33
		-mfpu=fpv5-sp-d16
		-mthumb
		-ffunction-sections
		-fdata-sections
		>

		$<$<CONFIG:Release>:
		-Wall
		-Wextra
		-Winline
		#-Werror
		-Wno-error=old-style-declaration
		-O3
		-s
		-g0
		-march=armv8-m.main
		-mtune=cortex-m33
		-mfpu=fpv5-sp-d16
		-mthumb
		-ffunction-sections
		-fdata-sections
		>
)

target_compile_options(NuclearBriefcase PRIVATE
		$<$<COMPILE_LANGUAGE:C>:-Wno-old-style-declaration>
)

target_link_options(NuclearBriefcase PRIVATE
		-march=armv8-m.main
		-mtune=cortex-m33
		-mfpu=fpv5-sp-d16
		-mthumb
		-Wl,--gc-sections
)

# Generate PIO headers
pico_generate_pio_header(NuclearBriefcase ${CMAKE_CURRENT_LIST_DIR}/modules/numbers/pio_numbers.pio)
pico_generate_pio_header(NuclearBriefcase ${CMAKE_CURRENT_LIST_DIR}/modules/wsleds/pio_wsleds.pio)

# Modify the below lines to enable/disable output over UART/USB
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	pico_enable_stdio_uart(NuclearBriefcase 1)
	pico_enable_stdio_usb(NuclearBriefcase 0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	pico_enable_stdio_uart(NuclearBriefcase 0)
	pico_enable_stdio_usb(NuclearBriefcase 1)
else()
	message(WARNING "Unhandled build type: ${CMAKE_BUILD_TYPE}")
endif()

# Add the standard library to the build
target_link_libraries(NuclearBriefcase
		pico_stdlib)

# Add the standard include files to the build
target_include_directories(NuclearBriefcase PRIVATE
		${CMAKE_CURRENT_LIST_DIR}
		${CMAKE_CURRENT_LIST_DIR}/../lib/pico-shared
)

add_subdirectory(../lib/pico-shared ${CMAKE_BINARY_DIR}/pico-shared)
target_link_libraries(NuclearBriefcase pico-shared)

# Add any user requested libraries
target_link_libraries(NuclearBriefcase
		hardware_i2c
		hardware_pio
		hardware_adc
		hardware_dma
		hardware_pwm
		pico_rand
		$<$<CONFIG:Debug>:hardware_uart>
)

pico_add_extra_outputs(NuclearBriefcase)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
add_custom_command(
		TARGET NuclearBriefcase
		POST_BUILD
		COMMAND /usr/bin/picotool load -f NuclearBriefcase.uf2
)
endif ()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(
			TARGET NuclearBriefcase
			POST_BUILD
			COMMAND /usr/bin/openocd  -s tcl -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c "adapter speed 5000" -c "program NuclearBriefcase.elf verify reset exit"
	)
endif ()

