#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

add_library(tfm_app_rot_partition_sp STATIC)

add_definitions(-DNRF -DTFM)

target_include_directories(tfm_app_rot_partition_sp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_BINARY_DIR}/secure_partition
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/secure_partition
)

target_include_directories(tfm_app_rot_partition_sp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

target_sources(tfm_app_rot_partition_sp
    PRIVATE
        secure_partition.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/common.c
)

# The generated sources
target_sources(tfm_app_rot_partition_sp
    PRIVATE
            ${CMAKE_BINARY_DIR}/secure_partition/auto_generated/intermedia_tfm_secure_partition.c>
)
target_sources(tfm_partitions
    INTERFACE
            ${CMAKE_BINARY_DIR}/secure_partition/auto_generated/load_info_tfm_secure_partition.c>
)

target_link_libraries(tfm_app_rot_partition_sp
    PRIVATE
        psa_interface
        secure_fw
        platform_s
        tfm_sprt
        tfm_psa_rot_partition_crypto
)

target_link_libraries(tfm_app_rot_partition_sp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libblst.a
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_app_rot_partition_sp
)

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_app_rot_partition_sp
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_SECURE_PARTITION
)
