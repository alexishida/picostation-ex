
cmake_minimum_required(VERSION 3.25)

# Internal MIPS build, invoked by the root firmware build.
get_filename_component(PICOSTATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(CMAKE_TOOLCHAIN_FILE "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/cmake/toolchain.cmake")
project(picostation-menu VERSION 1.0.0 LANGUAGES C CXX ASM)

# Set up compiler flags and initialize the Python environment used to run the
# scripts in the tools directory.
include("${PICOSTATION_ROOT}/third_party/ps1-bare-metal/cmake/setup.cmake")
find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
find_program(MKPSXISO_EXECUTABLE mkpsxiso REQUIRED)

# Build a "common" library containing basic support code. We are going to link
# this library into our executable.
add_library(
    common OBJECT
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/clz.s"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/crt0.c"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/cxxsupport.cpp"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/malloc.c"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/misc.c"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/setjmp.s"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/string.c"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc/string.s"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/ps1/cache.s"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/vendor/printf.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/system.s"
    "${PICOSTATION_ROOT}/src/menu/psxproject/unhandledexc.c"
)
target_include_directories(
    common PUBLIC
    "${PICOSTATION_ROOT}/include/menu"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal"
    "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/libc"
)
# The menu uses 32-bit formatting. Avoid pulling a 64-bit division helper
# from toolchains whose libgcc targets a newer MIPS processor than the PS1.
target_compile_definitions(common PRIVATE PRINTF_DISABLE_SUPPORT_LONG_LONG)

# Compile the main executable. You may add more source files by listing them
# here.
add_executable(
    ${PROJECT_NAME}
    "${PICOSTATION_ROOT}/src/menu/gpu.c"
    "${PICOSTATION_ROOT}/src/menu/main.c"
    "${PICOSTATION_ROOT}/src/menu/menu_media.c"
    "${PICOSTATION_ROOT}/src/menu/menu_game_id.c"
    "${PICOSTATION_ROOT}/src/menu/file_manager.c"
    "${PICOSTATION_ROOT}/src/menu/controller.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/cdrom.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/delay.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/filesystem.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/irq.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/system.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/stream.c"
    "${PICOSTATION_ROOT}/src/menu/psxproject/spu.c"
)
target_link_libraries(${PROJECT_NAME} PRIVATE common)
target_include_directories(${PROJECT_NAME} PRIVATE "${PICOSTATION_ROOT}/include")
target_compile_options(common PRIVATE $<$<CONFIG:Release>:-Os>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-Os>)

# Define a CMake macro that invokes convertImage.py in order to generate VRAM
# texture data from an image file.
function(convertImage input bpp)
    add_custom_command(
        OUTPUT  ${ARGN}
        DEPENDS "${PICOSTATION_ROOT}/assets/menu/${input}"
                "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/tools/convertImage.py"
        COMMAND
            "${Python3_EXECUTABLE}"
            "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/tools/convertImage.py"
            -b ${bpp}
            "${PICOSTATION_ROOT}/assets/menu/${input}"
            ${ARGN}
        VERBATIM
    )
endfunction()

# Convert the font spritesheet to a 4bpp texture and palette, then embed them
# into the executable. The addBinaryFile() macro is defined in setup.cmake; you
# may call it multiple times to embed other data into the binary.
convertImage(images/font.png 4 fontTexture.dat fontPalette.dat)
convertImage(images/picostationlogo.png 4 logoTexture.dat logoPalette.dat)
addBinaryFile(${PROJECT_NAME} fontTexture "${PROJECT_BINARY_DIR}/fontTexture.dat")
addBinaryFile(${PROJECT_NAME} fontPalette "${PROJECT_BINARY_DIR}/fontPalette.dat")
addBinaryFile(${PROJECT_NAME} logoTexture "${PROJECT_BINARY_DIR}/logoTexture.dat")
addBinaryFile(${PROJECT_NAME} logoPalette "${PROJECT_BINARY_DIR}/logoPalette.dat")
addBinaryFile(${PROJECT_NAME} click_sfx "${PICOSTATION_ROOT}/assets/menu/click.vag")
addBinaryFile(${PROJECT_NAME} slide_sfx "${PICOSTATION_ROOT}/assets/menu/slide.vag")

set(menu_executable "${CMAKE_CURRENT_BINARY_DIR}/picostation-menu.psexe")
set(menu_image "${CMAKE_CURRENT_BINARY_DIR}/picostation-menu.bin")
set(menu_cue "${CMAKE_CURRENT_BINARY_DIR}/picostation-menu.cue")
set(iso_config "${CMAKE_CURRENT_BINARY_DIR}/isoconfig.xml")
set(iso_assets "${PICOSTATION_ROOT}/assets/menu/xiso")
configure_file("${iso_assets}/isoconfig.xml" "${iso_config}" @ONLY)

add_custom_command(
    OUTPUT "${menu_executable}"
    COMMAND "${Python3_EXECUTABLE}"
            "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/tools/convertExecutable.py"
            --omit-zero-fill "$<TARGET_FILE:${PROJECT_NAME}>" "${menu_executable}"
    DEPENDS ${PROJECT_NAME} "${PICOSTATION_ROOT}/third_party/ps1-bare-metal/tools/convertExecutable.py"
    VERBATIM
)
add_custom_command(
    OUTPUT "${menu_image}" "${menu_cue}"
    COMMAND "${MKPSXISO_EXECUTABLE}" -y "${iso_config}" -o "${menu_image}" -c "${menu_cue}"
    DEPENDS "${menu_executable}" "${iso_config}" "${MKPSXISO_EXECUTABLE}"
            "${iso_assets}/LICENSEJ.DAT" "${iso_assets}/system.cnf" "${iso_assets}/PICO.DAT"
    VERBATIM
)
add_custom_target(picostation_menu_image ALL DEPENDS "${menu_image}" "${menu_cue}")
