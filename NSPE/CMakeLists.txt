cmake_minimum_required(VERSION 3.15)
set(TARGET_EXE nspe)
set(TAGET_PROJECT_DIR ${PROJECT_SOURCE_DIR}/NSPE/Project/WIFI_IOT)

add_executable(${TARGET_EXE})

if (CONFIG_MBEDTLS_VERSION STREQUAL "2.17.0")
    include_directories(
        ${PROJECT_SOURCE_DIR}/ROM-EXPORT/mbedtls-2.17.0-rom/include
    )
else ()
    include_directories(
        ${PROJECT_SOURCE_DIR}/NSPE/WIFI_IOT/network/mbedtls/include
        ${PROJECT_SOURCE_DIR}/NSPE/WIFI_IOT/network/mbedtls/library
    )
endif ()

add_subdirectory(Firmware/GD32W51x_standard_peripheral)
add_subdirectory(Firmware/GD32W51x_wifi_driver)
add_subdirectory(Firmware/CMSIS)
add_subdirectory(WIFI_IOT)

set_target_properties(${TARGET_EXE}
    PROPERTIES
        SUFFIX ".axf"
        ARCHIVE_OUTPUT_DIRECTORY "${TAGET_PROJECT_DIR}/GCC/output/lib"
        LIBRARY_OUTPUT_DIRECTORY "${TAGET_PROJECT_DIR}/GCC/output/lib"
        RUNTIME_OUTPUT_DIRECTORY "${TAGET_PROJECT_DIR}/GCC/output/bin"
    )

target_add_scatter_file(${TARGET_EXE}
    ${TAGET_PROJECT_DIR}/GCC/nspe_gdm32_ns.ld
    )

if (CONFIG_STATIC_LIB_BUILDED)

    add_library(gd32w51x_wifi STATIC IMPORTED GLOBAL)

    set_target_properties(gd32w51x_wifi
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/NSPE/WIFI_IOT/lib/GCC/libgd32w51x_wifi.a)
endif()

target_link_libraries(${TARGET_EXE}
    PRIVATE
        gd32w51x_wifi
    )

target_link_libraries(${TARGET_EXE}
    PRIVATE
        app
        gd32w51x_peripheral
        bsp
        common
        lwIP
        mbedtls
        wifi
        os
        cmsis
    )

target_link_options(${TARGET_EXE}
    PUBLIC
    -Wl,-Map=${TAGET_PROJECT_DIR}/GCC/output/bin/${TARGET_EXE}.map
    -Wl,--cref
)

if (CONFIG_MBEDTLS_VERSION STREQUAL "2.17.0")
    target_link_options(${TARGET_EXE}
        PUBLIC
        -Wl,--just-symbols=${PROJECT_SOURCE_DIR}/ROM-EXPORT/symbol/rom_symbol.gcc
    )
endif ()

if(${CMAKE_HOST_WIN32})
    add_custom_command(TARGET ${TARGET_EXE} POST_BUILD
        COMMAND ${TAGET_PROJECT_DIR}/nspe_afterbuild.bat ${TAGET_PROJECT_DIR}/GCC/output/bin GCC \"\"
        COMMENT "Building ${TARGET_EXE}"
    )
elseif(${CMAKE_HOST_UNIX})
    add_custom_command(TARGET ${TARGET_EXE} POST_BUILD
        COMMAND ${TAGET_PROJECT_DIR}/nspe_afterbuild.sh ${TAGET_PROJECT_DIR}/GCC/output/bin GCC \"\"
        COMMENT "Building ${TARGET_EXE}"
    )
else()
    message( FATAL_ERROR "Only support Windows platform or Linux platform" )
endif ()