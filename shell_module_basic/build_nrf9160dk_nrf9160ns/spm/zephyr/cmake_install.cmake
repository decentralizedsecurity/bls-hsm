# Install script for directory: C:/Users/pablo/ncs/v1.4.99-dev1/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Zephyr-Kernel")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/arch/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/lib/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/soc/arm/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/boards/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/subsys/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/drivers/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/nrf/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/mcuboot/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/mcumgr/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/nrfxlib/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/cmsis/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/canopennode/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/civetweb/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/fatfs/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/nordic/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/st/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/libmetal/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/lvgl/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/mbedtls/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/open-amp/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/loramac-node/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/openthread/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/segger/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/tinycbor/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/tinycrypt/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/littlefs/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/mipi-sys-t/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/nrf_hw_models/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/modules/tfm/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/kernel/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/cmake/flash/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/cmake/usage/cmake_install.cmake")
  include("C:/Users/pablo/ncs/v1.4.99-dev1/zephyr/samples/subsys/shell/shell_module_basic/build_nrf9160dk_nrf9160ns/spm/zephyr/cmake/reports/cmake_install.cmake")

endif()

