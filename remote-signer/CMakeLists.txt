# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
project(remote_signer)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

get_target_property(TFM_BINARY_DIR tfm TFM_BINARY_DIR)
configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/../secure_partition/tfm_manifest_list.yaml.in
  ${CMAKE_CURRENT_BINARY_DIR}/../../secure_partition/tfm_manifest_list.yaml
)

set_property(TARGET zephyr_property_target
  APPEND PROPERTY TFM_CMAKE_OPTIONS
  -DTFM_EXTRA_MANIFEST_LIST_FILES=${CMAKE_CURRENT_BINARY_DIR}/../../secure_partition/tfm_manifest_list.yaml
  -DTFM_EXTRA_PARTITION_PATHS=${CMAKE_CURRENT_LIST_DIR}/../secure_partition
)

add_definitions(-DNRF -DNRF_RS)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources}
  ../lib/picohttpparser.c
  ../lib/bls_hsm_ns.c
  ../lib/common.c
  ../lib/secure_partition.c
)

target_include_directories(app PRIVATE
  $<TARGET_PROPERTY:tfm,TFM_BINARY_DIR>/install/interface/include
)

target_compile_definitions(app
    PRIVATE TFM_PARTITION_SECURE_PARTITION
)

add_library(testlib STATIC IMPORTED)
target_link_libraries(app PUBLIC ${APPLICATION_SOURCE_DIR}/../lib/libblst.a)