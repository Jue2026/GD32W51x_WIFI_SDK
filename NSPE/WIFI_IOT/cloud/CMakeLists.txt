
cmake_minimum_required(VERSION 3.15)

add_library(alicloud_api INTERFACE)

add_library(alicloud)

SET(ali_linkkit_src ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/src)

file(GLOB_RECURSE SRC_SDK  ${ali_linkkit_src}/*.c)

list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/protocol/coap/cloud*")
list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/ref-impl/hal/os/macos*")
list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/ref-impl/hal/os/ubuntu*")
list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/ref-impl/hal/os/win7*")
list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/ref-impl/hal/ssl*")
list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/ref-impl/tls*")
list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/services/mdal*")
list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/tools*")

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list  ${ali_linkkit_src}/*.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(header_list)

add_definitions(
    -DSUPPORT_TLS
    -DCOAP_SERV_MULTITHREAD
    -DAWSS_SUPPORT_APLIST
    -DMQTT_AUTO_SUBSCRIBE
    -DMQTT_COMM_ENABLED
    -DDEVICE_MODEL_ENABLED
    -DDEV_BIND_ENABLED
    -DOTA_ENABLED
    -DWIFI_PROVISION_ENABLED
    -DAWSS_SUPPORT_DEV_AP
    -DAWSS_SUPPORT_SMARTCONFIG
)

target_sources(alicloud
    PRIVATE
        # alicloud_entry.c
        # linkkit_example_solo.c
        ${SRC_SDK}
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/examples/linkkit/living_platform/alicloud_entry.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/examples/linkkit/living_platform/living_platform_main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/examples/linkkit/living_platform/living_platform_awss_reset.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/examples/linkkit/living_platform/living_platform_ut.c
)

target_include_directories(alicloud
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/include
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/include/exports
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/include/imports
)

target_include_directories(alicloud_api
    INTERFACE
        ${header_list}
        ${ali_linkkit_src}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/alicloud/examples/linkkit/living_platform
)

target_link_libraries(alicloud
    alicloud_api
    os_api
    gd32w51x_peripheral_api
    gd32w51x_wifi_api
    mbedtls_ssl
    lwIP_api
    wifi_api
)
