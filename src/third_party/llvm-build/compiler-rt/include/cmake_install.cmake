# Install script for directory: /media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/sanitizer/allocator_interface.h;/usr/local/include/sanitizer/asan_interface.h;/usr/local/include/sanitizer/common_interface_defs.h;/usr/local/include/sanitizer/coverage_interface.h;/usr/local/include/sanitizer/dfsan_interface.h;/usr/local/include/sanitizer/linux_syscall_hooks.h;/usr/local/include/sanitizer/lsan_interface.h;/usr/local/include/sanitizer/msan_interface.h;/usr/local/include/sanitizer/tsan_interface_atomic.h")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/usr/local/include/sanitizer" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/allocator_interface.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/asan_interface.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/common_interface_defs.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/coverage_interface.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/dfsan_interface.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/linux_syscall_hooks.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/lsan_interface.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/msan_interface.h"
    "/media/mengxk/data/sources/chromium-googlecode-origin/src/third_party/llvm/compiler-rt/include/sanitizer/tsan_interface_atomic.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

