set(PICOSTATION_PINOUT_FILE "${CMAKE_CURRENT_LIST_DIR}/${PICOSTATION_VARIANT}.cmake")
if(PICOSTATION_VARIANT MATCHES "^picostationex_PU(8|18)(_OC)?$")
    include("${CMAKE_CURRENT_LIST_DIR}/picostationex_release.cmake")
elseif(EXISTS "${PICOSTATION_PINOUT_FILE}")
    include(${PICOSTATION_PINOUT_FILE})
else()
    message(FATAL_ERROR "Pinout file not found: ${PICOSTATION_PINOUT_FILE}")
endif()

message(STATUS "Building for ${PICOSTATION_VARIANT}.")

configure_file(${CMAKE_CURRENT_LIST_DIR}/picostation_pinout.h.in picostation_pinout.h @ONLY)

set(PROJECT_NAME "${PICOSTATION_VARIANT}")
