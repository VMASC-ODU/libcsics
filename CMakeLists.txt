cmake_minimum_required(VERSION 3.10)
project(csics VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


option(CSICS_BUILD_ALL "Build all modules" ON)
option(CSICS_BUILD_QUEUE "Build the queue module" ${CSICS_BUILD_ALL})
option(CSICS_BUILD_RADIO "Build the radio module" ${CSICS_BUILD_ALL})
option(CSICS_BUILD_IO "Build the IO module" ${CSICS_BUILD_ALL})
option(CSICS_USE_UHD "Use the UHD library for USRP support" ${CSICS_BUILD_RADIO})
option(CSICS_USE_ZSTD "Use the ZSTD library for compression support" ${CSICS_BUILD_IO})
option(CSICS_USE_ZLIB "Use the ZLIB library for compression support" ${CSICS_BUILD_IO})
option(CSICS_ENABLE_TESTS "Enable building tests" ${CSICS_BUILD_ALL})

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(CSICS_COMPILE_DEFINITIONS)
include(cmake/deps.cmake)

set(COMPONENTS CSICS::core)

if (CSICS_BUILD_QUEUE)
    list(APPEND COMPONENTS CSICS::queue)
endif()

if (CSICS_BUILD_RADIO)
    list(APPEND COMPONENTS CSICS::radio)
endif()

if (CSICS_BUILD_IO)
    list(APPEND COMPONENTS CSICS::io)
endif()

add_subdirectory(src)

add_library(CSICS INTERFACE)
target_include_directories(CSICS INTERFACE ${INCLUDE_DIR})
target_link_libraries(CSICS INTERFACE ${COMPONENTS})
target_compile_definitions(CSICS INTERFACE ${CSICS_COMPILE_DEFINITIONS})

message(STATUS "CSICS build configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Components: ${COMPONENTS}")
message(STATUS "  UHD support: ${CSICS_USE_UHD}")

if (CSICS_ENABLE_TESTS)
    add_subdirectory(test)
endif()
