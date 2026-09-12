cmake_minimum_required(VERSION 3.13...3.27)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(UHBadge C CXX ASM)
pico_sdk_init()

add_executable(UHBadge
	main.c
	firmware.c
	ui.c
	buttons.c
)
