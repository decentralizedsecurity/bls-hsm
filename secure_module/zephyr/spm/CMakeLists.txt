#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)

zephyr_include_directories(
  kernel/include
  ${ARCH_DIR}/${ARCH}/include
)

zephyr_library_sources(src/main.c)
zephyr_library_include_directories(
  ../../../lib/
  )

add_library(testlib STATIC IMPORTED)
target_link_libraries(
	app PUBLIC ${APPLICATION_SOURCE_DIR}/../../../lib/libblst.a
	nrfxlib_crypto
)