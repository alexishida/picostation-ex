cmake_minimum_required(VERSION 3.25)

get_filename_component(project_dir "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(build_dir "${project_dir}/build")
set(common_args -DCMAKE_BUILD_TYPE=Release)
foreach(setting IN ITEMS PICO_SDK_PATH PICO_TOOLCHAIN_PATH picotool_DIR)
    if(DEFINED ${setting})
        list(APPEND common_args "-D${setting}=${${setting}}")
    endif()
endforeach()

set(ode_args ${common_args} -DPICOSTATION_BUILD_MENU=ON)
foreach(setting IN ITEMS PICOSTATION_MIPS_TOOLCHAIN_PATH Python3_EXECUTABLE MKPSXISO_EXECUTABLE)
    if(DEFINED ${setting})
        list(APPEND ode_args "-D${setting}=${${setting}}")
    endif()
endforeach()

set(variants picostationex_PU8 picostationex_PU8_OC picostationex_PU18 picostationex_PU18_OC)
foreach(variant IN LISTS variants)
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -S "${project_dir}" -B "${build_dir}/${variant}"
                "-DPICOSTATION_VARIANT=${variant}" ${ode_args}
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${build_dir}/${variant}" --config Release
        COMMAND_ERROR_IS_FATAL ANY
    )
    file(COPY_FILE "${build_dir}/${variant}/${variant}.uf2" "${build_dir}/${variant}.uf2"
         ONLY_IF_DIFFERENT)
endforeach()

find_program(PS1_PACKER_EXECUTABLE NAMES ps1-packer REQUIRED)
execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${project_dir}/cmake/pico-booty" -B "${build_dir}/pico_booty"
            "-DPICOSTATION_MENU_EXE=${build_dir}/picostationex_PU8/menu/picostation-menu.psexe"
            "-DPS1_PACKER_EXECUTABLE=${PS1_PACKER_EXECUTABLE}" ${common_args}
    COMMAND_ERROR_IS_FATAL ANY
)
execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${build_dir}/pico_booty" --config Release
    COMMAND_ERROR_IS_FATAL ANY
)
file(COPY_FILE "${build_dir}/pico_booty/pico_booty.uf2" "${build_dir}/pico_booty.uf2"
     ONLY_IF_DIFFERENT)
