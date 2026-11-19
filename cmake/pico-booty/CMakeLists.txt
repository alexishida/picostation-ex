cmake_minimum_required(VERSION 3.25)

get_filename_component(PICOSTATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
if(NOT DEFINED PICO_SDK_PATH AND NOT DEFINED ENV{PICO_SDK_PATH}
   AND EXISTS "${PICOSTATION_ROOT}/third_party/pico-sdk/pico_sdk_init.cmake")
    set(PICO_SDK_PATH "${PICOSTATION_ROOT}/third_party/pico-sdk")
endif()
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)
include("${PICOSTATION_ROOT}/cmake/pico_sdk_import.cmake")

project(pico_booty LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
pico_sdk_init()

if(NOT EXISTS "${PICOSTATION_MENU_EXE}")
    message(FATAL_ERROR "Set PICOSTATION_MENU_EXE to the menu PS-X EXE produced by an EX source build.")
endif()
find_program(PS1_PACKER_EXECUTABLE NAMES ps1-packer REQUIRED)

set(menu_payload "${CMAKE_CURRENT_BINARY_DIR}/picostation-menu.booty")
add_custom_command(
    OUTPUT "${menu_payload}"
    COMMAND "${PS1_PACKER_EXECUTABLE}" "${PICOSTATION_MENU_EXE}" -booty -o "${menu_payload}"
    DEPENDS "${PICOSTATION_MENU_EXE}" "${PS1_PACKER_EXECUTABLE}"
    VERBATIM
)
set(payload_asm "${CMAKE_CURRENT_BINARY_DIR}/payload.s")
file(CONFIGURE OUTPUT "${payload_asm}" CONTENT [[
    .section .data
    .balign 4
    .global payload_start_addr, payload_end_addr
payload_start_addr:
    .incbin "@menu_payload@"
payload_end_addr:
]] @ONLY)
set_source_files_properties("${payload_asm}" PROPERTIES OBJECT_DEPENDS "${menu_payload}")
add_custom_target(pico_booty_menu DEPENDS "${menu_payload}")

set(booty_source "${PICOSTATION_ROOT}/third_party/pico_booty")
add_executable(pico_booty "${booty_source}/pico_booty.cpp" "${payload_asm}")
add_dependencies(pico_booty pico_booty_menu)
target_link_libraries(pico_booty PRIVATE pico_stdlib hardware_dma hardware_pio)
pico_generate_pio_header(pico_booty "${booty_source}/parallel.pio")
pico_set_program_name(pico_booty "pico_booty")
pico_enable_stdio_uart(pico_booty 0)
pico_enable_stdio_usb(pico_booty 1)
pico_add_extra_outputs(pico_booty)
