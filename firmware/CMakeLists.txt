cmake_minimum_required(VERSION 3.13...3.27)

set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk)

set(PICO_BOARD pico2_w)

list(APPEND PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(UHBadge C CXX ASM)

pico_sdk_init()

add_executable(UHBadge
	main.c
	firmware.c
	ui/framebuffer.c
	ui/eink.c
	ui/ui.c
	tools/buttons.c
	tools/nfc.c
	tools/storage.c
	tools/wifi.c
	tools/sd.c
	tools/diskio.c
	lib/fatfs/ff.c
	lib/fatfs/ffunicode.c
)

target_include_directories(UHBadge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/lib/fatfs
)

target_link_libraries(UHBadge
    pico_stdlib
    hardware_spi
    hardware_gpio
	hardware_i2c
	pico_cyw43_arch_lwip_poll
)

pico_add_extra_outputs(UHBadge)
