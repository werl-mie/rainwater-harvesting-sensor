# Install script for directory: /opt/nordic/ncs/v3.0.0/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/nordic/ncs/toolchains/b8efef2ad5/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/suit-processor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/tinycrypt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/erudy/werl/post-masc/thingy91/nrf_cloud_multi_service/build_provision/nrf_cloud_multi_service/zephyr/cmake/reports/cmake_install.cmake")
endif()

