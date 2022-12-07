#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

get_target_property(TFM_BINARY_DIR tfm TFM_BINARY_DIR)
configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/../secure_partition/tfm_manifest_list.yaml.in
  ${CMAKE_CURRENT_BINARY_DIR}/secure_partition/tfm_manifest_list.yaml
)

set_property(TARGET zephyr_property_target
  APPEND PROPERTY TFM_CMAKE_OPTIONS
  -DTFM_EXTRA_MANIFEST_LIST_FILES=${CMAKE_CURRENT_BINARY_DIR}/secure_partition/tfm_manifest_list.yaml
  -DTFM_EXTRA_PARTITION_PATHS=${CMAKE_CURRENT_LIST_DIR}/../secure_partition
)

project(mycli)
add_definitions(-DNRF -DTFM)

target_sources(app PRIVATE
  src/main.c
  ../lib/secure_partition_interface.c
  ../lib/bls_hsm_ns.c
  ../lib/common.c
)

zephyr_library_include_directories(
  ../lib/
)

add_library(testlib STATIC IMPORTED)
target_link_libraries(app PUBLIC ${APPLICATION_SOURCE_DIR}/../lib/libblst.a)

target_include_directories(app PRIVATE
  $<TARGET_PROPERTY:tfm,TFM_BINARY_DIR>/install/interface/include
)

target_compile_definitions(app
    PRIVATE TFM_PARTITION_DUMMY_PARTITION
)

# To see type of fatal error in the log 
set_property(
TARGET zephyr_property_target
APPEND PROPERTY TFM_CMAKE_OPTIONS

# Use -O0. So GDB reports correct line numbers.
-DCMAKE_BUILD_TYPE=Debug

# Halt instead of rebooting on internal TF-M faults.
-DTFM_HALT_ON_CORE_PANIC=ON

# NB: Probably not enough MPU regions on nrf53 in Isolation level 2
-DNULL_POINTER_EXCEPTION_DETECTION=ON

# Add a debug function that logs the memory protection config, and
# then invoke it from the SPE with log_memory_protection() to see
# the memory protection configuration.
-DLOG_MEMORY_PROTECTION=ON

# WDT will force tests to continue for certain types of TF-M
# tests. This can be inconvenient when debugging. But the WDT can
# also be a necessary part of the test execution.
# -DWATCHDOG_AVAILABLE=0
)