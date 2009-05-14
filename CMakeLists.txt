cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)
project(TUT)

include(CheckFunctionExists)
include(CheckIncludeFile)

if(UNIX)
    OPTION(TUT_USE_FORK "Enable multi-process testing" ON) 
    OPTION(TUT_USE_REFLECTION "Enable reflection" ON) 
else(UNIX)
    OPTION(TUT_USE_FORK "Enable multi-process testing" OFF) 
    OPTION(TUT_USE_REFLECTION "Enable reflection" OFF) 
endif(UNIX)

if(WINDOWS)
    OPTION(TUT_USE_SEH "Enable SEH support for Win32" ON) 
else(WINDOWS)
    OPTION(TUT_USE_SEH "Enable SEH support for Win32" OFF) 
endif(WINDOWS)

IF(TUT_USE_FORK)
    MESSAGE(STATUS "Multi-process testing: enabled")
    SET(TUT_USE_POSIX 1)
    
    SET(CMAKE_EXTRA_INCLUDE_FILES unistd.h)
    CHECK_FUNCTION_EXISTS(fork HAVE_FORK) 
    SET(CMAKE_EXTRA_INCLUDE_FILES)

    if(NOT HAVE_FORK)
        MESSAGE(FATAL_ERROR "Cannot find fork()")
    endif(NOT HAVE_FORK) 
ELSE(TUT_USE_FORK)
    MESSAGE(STATUS "Multi-process testing: disabled")
ENDIF(TUT_USE_FORK)

IF(TUT_USE_REFLECTION)
    MESSAGE(STATUS "Reflection: enabled")
    SET(TUT_USE_FUNCTIONS 1)

    add_subdirectory(libreflection)
ELSE(TUT_USE_REFLECTION)
    MESSAGE(STATUS "Reflection: disabled")
ENDIF(TUT_USE_REFLECTION)

IF(TUT_USE_SEH)
    MESSAGE(STATUS "SEH support: enabled")
    SET(TUT_USE_SEH 1)

    CHECK_INCLUDE_FILE(windows.h HAVE_WINDOWS_H) 

    if(NOT HAVE_WINDOWS_H)
        MESSAGE(FATAL_ERROR "Cannot find windows.h")
    endif(NOT HAVE_WINDOWS_H) 
ELSE(TUT_USE_SEH)
    MESSAGE(STATUS "SEH support: disabled")
ENDIF(TUT_USE_SEH)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tut_config.in ${CMAKE_CURRENT_BINARY_DIR}/include/tut/tut_config.hpp)

INCLUDE_DIRECTORIES(./include ./libreflection/include)

add_subdirectory(selftest)
add_subdirectory(examples)
add_subdirectory(tutgen)

# add_subdirectory(plugins/distributed)

