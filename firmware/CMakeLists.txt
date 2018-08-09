#-----------------------------------------------------------------------------
# CmakeLists.txt for the Digital Bitbox 
# Copyright (c) 2015-2016 Lucas Betschart, Douglas J. Bakkum
# MIT License


cmake_minimum_required(VERSION 2.8.5)

    
set(MYPROJECT "firmware")
project(${MYPROJECT} C)


#-----------------------------------------------------------------------------
# Options for building

option(CMAKE_VERBOSE_MAKEFILE "Verbose build." OFF)

set(ELF ${MYPROJECT}.elf)
set(CMAKE_TOOLCHAIN_FILE arm.cmake)
include(${CMAKE_TOOLCHAIN_FILE})

execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE C_COMPILER_VERSION)


#-----------------------------------------------------------------------------
# Force out-of-source build

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
message(FATAL_ERROR "CMake generation is not allowed within the source directory!
Remove the CMakeCache.txt file and try again from another folder, e.g.:

   rm CMakeCache.txt
   mkdir build
   cd build
   cmake ..
")
endif()


#-----------------------------------------------------------------------------
# Print system information and build options

message(STATUS "\n\n=============================================")
message(STATUS "            - General -")
message(STATUS "MCU version:            ${VERSION_STRING}")
message(STATUS "CMake version:          ${CMAKE_VERSION}")
message(STATUS "System:                 ${CMAKE_SYSTEM}")
message(STATUS "Processor:              ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "             - Build -")
message(STATUS "Compiler version:       ${CMAKE_C_COMPILER_ID} ${C_COMPILER_VERSION}")
message(STATUS "Compiler:               ${CMAKE_C_COMPILER}")
message(STATUS "Linker:                 ${CMAKE_LINKER}")
message(STATUS "Archiver:               ${CMAKE_AR}")
message(STATUS "\n=============================================\n\n")


#-----------------------------------------------------------------------------
# Collect all binaries into bin subdirectory

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR}/lib)


#-----------------------------------------------------------------------------
# Compiler

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -std=gnu99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb -fomit-frame-pointer -ffunction-sections -fdata-sections -mlong-calls -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -pipe -DNDEBUG -D__SAMD51J20A__") 


#-----------------------------------------------------------------------------
# Build 

message(STATUS "C Flags:          ${CMAKE_C_FLAGS}")

add_subdirectory(src)

add_custom_command(
    OUTPUT ${MYPROJECT}.bin
    COMMAND ${CMAKE_SIZE} ${ELF}
    COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF} ${MYPROJECT}.bin
    DEPENDS ${ELF}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
    COMMENT "\nGenerating binary"
)
add_custom_target(binary_file ALL
    DEPENDS ${MYPROJECT}.bin ${ELF}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)
add_dependencies(binary_file ${ELF})


#-----------------------------------------------------------------------------
# Clean 

set(removefiles "bin/*.* lib/*.*")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${removefiles}")
