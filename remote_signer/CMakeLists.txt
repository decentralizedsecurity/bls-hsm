# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
list(APPEND ZEPHYR_EXTRA_MODULES
   ${CMAKE_CURRENT_SOURCE_DIR}/../secure_module)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(remote_signer)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources}
  ../include/picohttpparser.c
  ../include/bls_hsm.c
  ../include/common.c
)

zephyr_library_include_directories(
  ../include/
)

add_library(testlib STATIC IMPORTED)
target_link_libraries(app PUBLIC ${APPLICATION_SOURCE_DIR}/../lib/libblst.a)