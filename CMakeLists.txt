# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
if(NOT DEFINED PICO_SDK_PATH AND NOT DEFINED ENV{PICO_SDK_PATH}
   AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/third_party/pico-sdk/pico_sdk_init.cmake")
    set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/third_party/pico-sdk")
endif()
if(NOT DEFINED PICO_SDK_FETCH_FROM_GIT_TAG AND NOT DEFINED ENV{PICO_SDK_FETCH_FROM_GIT_TAG})
    set(PICO_SDK_FETCH_FROM_GIT_TAG "${sdkVersion}")
endif()
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Example override variant
# set(PICOSTATION_VARIANT "picostation_plus_pico2")

if(NOT DEFINED PICOSTATION_VARIANT)
    if(PICO_BOARD STREQUAL "pico2")
        set(PICOSTATION_VARIANT "picostation_pico2")
    else()
        set(PICOSTATION_VARIANT "picostation_pico1")
    endif()
    message(STATUS "NOTE: PicoStation variant not defined. Using ${PICOSTATION_VARIANT}")
endif()

include(boards/picostation_variant.cmake)

cmake_minimum_required(VERSION 3.24.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/pico_sdk_import.cmake)

project(${PROJECT_NAME} VERSION 1.0.0)

pico_sdk_init()

set(CMAKE_BUILD_TYPE Release)

function(addBinaryFileWithSize target name sizeName path)
    set(asmFile "${PROJECT_BINARY_DIR}/includes/${target}_${name}.s")
    cmake_path(ABSOLUTE_PATH path OUTPUT_VARIABLE fullPath)

    file(
        CONFIGURE
        OUTPUT "${asmFile}"
        CONTENT [[
            .section .rodata.${name}, "a"
            .balign 8

            .global ${name}
            .type ${name}, %object
            .size ${name}, (${name}_end - ${name})

            ${name}:
                .incbin "${fullPath}"
            ${name}_end:

            .section .rodata.${sizeName}, "a"
            .balign 4

            .global ${sizeName}
            .type ${sizeName}, %object
            .size ${sizeName}, 4

            ${sizeName}:
                .int (${name}_end - ${name})
        ]]
        ESCAPE_QUOTES
        NEWLINE_STYLE LF
    )

    target_sources(${target} PRIVATE "${asmFile}")
    set_source_files_properties(
        "${asmFile}" PROPERTIES OBJECT_DEPENDS "${fullPath}"
    )
endfunction()

add_executable(${PROJECT_NAME})

option(PICOSTATION_SD_BOOT_LOG "Write boot diagnostics to erro.log on the SD card" OFF)
if(PICOSTATION_SD_BOOT_LOG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PICOSTATION_SD_BOOT_LOG=1)
endif()

target_compile_definitions(
    ${PROJECT_NAME} PUBLIC
    PICO_DEFAULT_UART=0
    PICO_DEFAULT_UART_TX_PIN=0
    PICO_DEFAULT_UART_RX_PIN=1
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
    PICO_CORE1_STACK_SIZE=4096
    MAXINDEX=2
)

option(PICOSTATION_BUILD_MENU "Build the integrated PlayStation menu from source" ON)
if(PICOSTATION_BUILD_MENU)
    if(CMAKE_VERSION VERSION_LESS 3.25)
        message(FATAL_ERROR "Building the PlayStation menu requires CMake 3.25 or newer")
    endif()
    include(ExternalProject)
    find_program(MKPSXISO_EXECUTABLE mkpsxiso REQUIRED)
    find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
    set(PICOSTATION_MIPS_TOOLCHAIN_PATH "" CACHE PATH "Directory containing the PlayStation GCC toolchain")
    set(menu_build_dir "${CMAKE_CURRENT_BINARY_DIR}/menu")
    set(menu_image "${menu_build_dir}/picostation-menu.bin")
    ExternalProject_Add(
        picostation_menu
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/menu"
        BINARY_DIR "${menu_build_dir}"
        CMAKE_ARGS
            "-DCMAKE_BUILD_TYPE:STRING=Release"
            "-DTOOLCHAIN_PATH:PATH=${PICOSTATION_MIPS_TOOLCHAIN_PATH}"
            "-DPython3_EXECUTABLE:FILEPATH=${Python3_EXECUTABLE}"
            "-DMKPSXISO_EXECUTABLE:FILEPATH=${MKPSXISO_EXECUTABLE}"
        BUILD_ALWAYS TRUE
        BUILD_BYPRODUCTS "${menu_image}"
        INSTALL_COMMAND ""
    )
    add_dependencies(${PROJECT_NAME} picostation_menu)
else()
    set(PICOSTATION_MENU_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/binary/picostation-menu.bin"
        CACHE FILEPATH "Path to a precompiled PlayStation menu disc image")
    get_filename_component(menu_image "${PICOSTATION_MENU_IMAGE}" ABSOLUTE
        BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT EXISTS "${menu_image}" OR IS_DIRECTORY "${menu_image}")
        message(FATAL_ERROR
            "Precompiled menu image not found: ${menu_image}. "
            "Set PICOSTATION_MENU_IMAGE to an existing menu .bin file, "
            "or enable PICOSTATION_BUILD_MENU to build the integrated menu.")
    endif()
endif()

addBinaryFileWithSize(${PROJECT_NAME} loaderImage loaderImageSize "${menu_image}")

target_sources(
    ${PROJECT_NAME} PRIVATE
    src/drive/cmd.cpp
    src/storage/disc_image.cpp
    src/drive/drive_mechanics.cpp
    src/disc/edc.c
    src/hardware/i2s.cpp
    src/main.cpp
    src/hardware/modchip.cpp
    src/drive/picostation.cpp
    src/disc/subq.cpp
    src/storage/directory_listing.cpp
    src/hardware/si5351.c
    third_party/cueparser/cueparser.c
    third_party/cueparser/fileabstract.c
    third_party/cueparser/scheduler.c
    third_party/posix_file.c
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/third_party
)

add_subdirectory(third_party/SD-fatfs build)

add_subdirectory(third_party/RP2040_Pseudo_Atomic)

option(PICOSTATION_USB_LOG "Enable USB serial logging" OFF)
pico_enable_stdio_usb(${PROJECT_NAME} ${PICOSTATION_USB_LOG})
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio/main.pio)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    hardware_dma
    hardware_pio
    hardware_pwm
    hardware_vreg
    hardware_i2c
    SD-fatfs
    pico_bootrom
    pico_multicore
    pico_stdlib
)

target_include_directories(SD-fatfs BEFORE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/source
)

pico_add_extra_outputs(${PROJECT_NAME})
