
# Simple arch detection
IF(${CMAKE_CXX_SIZEOF_DATA_PTR} EQUAL 4)
SET(CMAKE_SYSTEM_ARCH x86)
ELSEIF(${CMAKE_CXX_SIZEOF_DATA_PTR} EQUAL 8)
SET(CMAKE_SYSTEM_ARCH x86_64)
ELSE(${CMAKE_CXX_SIZEOF_DATA_PTR} EQUAL 8)
MESSAGE(FATAL_ERROR "Unknown architecture!")
ENDIF(${CMAKE_CXX_SIZEOF_DATA_PTR} EQUAL 4)

MESSAGE(STATUS "Reflection architecture: ${CMAKE_SYSTEM_ARCH}")

FILE(GLOB reflection_FILES "*.cpp")
FILE(GLOB reflection_arch_FILES "arch/${CMAKE_SYSTEM_ARCH}/*.cpp")

ADD_LIBRARY(reflection STATIC ${reflection_FILES} ${reflection_arch_FILES})

## shared lib does not work - PIC code places restrictions on inline asm
# ADD_LIBRARY(libreflection DYNAMIC ${libreflection_FILES} ${libreflection_arch_FILES})

INCLUDE_DIRECTORIES(./include ${CMAKE_SOURCE_DIR}/include)
FILE(GLOB reflection_HEADERS "include/reflection/*.h")
INSTALL(FILES ${reflection_HEADERS} DESTINATION "include/reflection")
INSTALL(TARGETS reflection
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
