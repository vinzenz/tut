cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)
project(TUT)

SET(BUILD_SHARED_LIBS 1)
INCLUDE_DIRECTORIES(./include ./libreflection/include)

add_subdirectory(selftest)
add_subdirectory(libreflection)
#add_subdirectory(examples)
#add_subdirectory(tutgen)

