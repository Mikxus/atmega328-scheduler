##########################################################################
# "THE ANY BEVERAGE-WARE LICENSE" (Revision 42 - based on beer-ware
# license):
# <dev@layer128.net> wrote this file. As long as you retain this notice
# you can do whatever you want with this stuff. If we meet some day, and
# you think this stuff is worth it, you can buy me a be(ve)er(age) in
# return. (I don't like beer much.)
#
# Matthias Kleemann
##########################################################################

cmake_minimum_required(VERSION 3.22)

### TOOLCHAIN SETUP AREA #################################################
# Set any variables used in the toolchain prior project() call. In that
# case they are already set and used.
##########################################################################

##########################################################################
# tools to beused for programming the AVR
##########################################################################
set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER avrispmkII)
set(AVR_UPLOADTOOL_PORT usb)

##########################################################################
# AVR and fuses needs to be set
##########################################################################
set(AVR_MCU atmega328p)
set(AVR_H_FUSE 0xd9)
set(AVR_L_FUSE 0xc3)

### END TOOLCHAIN SETUP AREA #############################################

include(submodules/cmake-avr/generic-gcc-avr.cmake)

##########################################################################
# name your project
##########################################################################
project(ATMEGA328-SCHEDULER)

##########################################################################
# status messages
##########################################################################
message(STATUS "Current build type is: ${CMAKE_BUILD_TYPE}")
message(STATUS "Current uploadtool is: ${AVR_UPLOADTOOL}")
message(STATUS "Current programmer is: ${AVR_PROGRAMMER}")
message(STATUS "Current upload port is: ${AVR_UPLOADTOOL_PORT}")
message(STATUS "Current uploadtool options are: ${AVR_UPLOADTOOL_OPTIONS}")
message(STATUS "Current MCU is set to: ${AVR_MCU}")
message(STATUS "Current H_FUSE is set to: ${AVR_H_FUSE}")
message(STATUS "Current L_FUSE is set to: ${AVR_L_FUSE}")

##########################################################################
# set build type
##########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

##########################################################################
# needs to be defined for AVR toolchain
##########################################################################
set(MCU_SPEED "16000000")

##########################################################################
# some cmake cross-compile necessities
##########################################################################

if(DEFINED ENV{AVR_FIND_ROOT_PATH})
    set(CMAKE_FIND_ROOT_PATH $ENV{AVR_FIND_ROOT_PATH})
else(DEFINED ENV{AVR_FIND_ROOT_PATH})
    if(EXISTS "/opt/local/avr")
      set(CMAKE_FIND_ROOT_PATH "/opt/local/avr")
    elseif(EXISTS "/usr/avr")
      set(CMAKE_FIND_ROOT_PATH "/usr/avr")
    elseif(EXISTS "/usr/lib/avr")
      set(CMAKE_FIND_ROOT_PATH "/usr/lib/avr")
    elseif(EXISTS "/usr/local/CrossPack-AVR")
      set(CMAKE_FIND_ROOT_PATH "/usr/local/CrossPack-AVR")
    else(EXISTS "/opt/local/avr")
      message(FATAL_ERROR "Please set AVR_FIND_ROOT_PATH in your environment.")
    endif(EXISTS "/opt/local/avr")
endif(DEFINED ENV{AVR_FIND_ROOT_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# not added automatically, since CMAKE_SYSTEM_NAME is "generic"
set(CMAKE_SYSTEM_INCLUDE_PATH "${CMAKE_FIND_ROOT_PATH}/include")
set(CMAKE_SYSTEM_LIBRARY_PATH "${CMAKE_FIND_ROOT_PATH}/lib")

##########################################################################
# status messages for generating
##########################################################################
message(STATUS "Set CMAKE_FIND_ROOT_PATH to ${CMAKE_FIND_ROOT_PATH}")
message(STATUS "Set CMAKE_SYSTEM_INCLUDE_PATH to ${CMAKE_SYSTEM_INCLUDE_PATH}")
message(STATUS "Set CMAKE_SYSTEM_LIBRARY_PATH to ${CMAKE_SYSTEM_LIBRARY_PATH}")
##########################################################################
# set compiler options for build types
##########################################################################

# default flags for avr builds
set(AVR_DEFAULT_FLAGS -fpack-struct -fshort-enums -Wall -funsigned-char -funsigned-bitfields -ffunction-sections -flto)

if(CMAKE_BUILD_TYPE MATCHES Release)
   set(CMAKE_C_FLAGS_RELEASE "-Os ")
   set(CMAKE_CXX_FLAGS_RELEASE "-Os -std=c++11")
   add_definitions("-DRELEASE_BUILD")
endif(CMAKE_BUILD_TYPE MATCHES Release)

if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
   set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Os -save-temps -g -gdwarf-3 -gstrict-dwarf")
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -save-temps -g -gdwarf-3 -gstrict-dwarf -std=c++11")
   add_definitions("-DRELWITHDEBINFO_BUILD")
endif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

if(CMAKE_BUILD_TYPE MATCHES Debug)
   set(CMAKE_C_FLAGS_DEBUG "-Og -save-temps -g -gdwarf-3 -gstrict-dwarf")
   set(CMAKE_CXX_FLAGS_DEBUG "-Og -save-temps -g -gdwarf-3 -gstrict-dwarf -w -std=c++11")
   add_definitions("-DDEBUG_BUILD")
endif(CMAKE_BUILD_TYPE MATCHES Debug)


# Also add mcu as compilie time definition
add_definitions(-DSCHEDULER_AVR_MCU=${AVR_MCU})

##########################################################################
# Custom flags for the project
##########################################################################
add_definitions("-DF_CPU=${MCU_SPEED}")


# Project configuration settings

# UART DRIVERS
set(
    CONF_UART_BAUD_RATE "921600UL"
)

# enable u2x mode for uart0
set(
    USART0_U2X_SPEED "0"
)

# Define uart definitions
if ({USART0_U2X_SPEED} STREQUAL "1")
  add_definitions("-DUSART0_U2X_SPEED")
endif()

add_definitions(
  "-DCONF_UART_BAUD_RATE=${CONF_UART_BAUD_RATE}"
)


# Task settings
set(
    CONF_TASK_NAME_MAX_LENGTH "15"
)
add_definitions(
  "-DCONF_TASK_NAME_MAX_LENGTH=${CONF_TASK_NAME_MAX_LENGTH}"
)

# track task cpu time
set(
    CONF_TRACK_TASK_CPU_TIME "1"
)
add_definitions(
  "-DCONF_TRACK_TASK_CPU_TIME=${CONF_TRACK_TASK_CPU_TIME}"
)

## scheduler settings
set(
  "CONF_SCHEDULER_ALGO_ROUND_ROBIN" "1"
)
add_definitions(
  "-DCONF_SCHEDULER_ALGO_ROUND_ROBIN=${CONF_SCHEDULER_ALGO_ROUND_ROBIN}"
)

message(STATUS "------- Project configurations -------")
message(STATUS "Uart driver[ ]:")
message(STATUS "            |- Baud rate: ${CONF_UART_BAUD_RATE}")
message(STATUS "            |- U2X mode enabled: ${CONF_UART_USE_U2X_MODE}")
message(STATUS "")
message(STATUS "Task settings[ ]:")
message(STATUS "              |- Name max length: ${CONF_TASK_NAME_MAX_LENGTH}")
message(STATUS "              |- Track cpu time: ${CONF_TRACK_TASK_CPU_TIME}")
message(STATUS "")
message(STATUS "Scheduler settings[ ]:")
message(STATUS "                   |- Round robin algo enabled: ${CONF_SCHEDULER_ALGO_ROUND_ROBIN}")
message(STATUS "------- End of configurations ---------")

##########################################################################
# include search paths
##########################################################################
include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/lib
)

##########################################################################
# building library and application in their subdirectories
##########################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/AVR-UART-LIB/)
add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/)

##########################################################################
# AVR tests using simavr
##########################################################################
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/test)

