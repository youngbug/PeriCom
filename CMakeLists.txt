cmake_minimum_required(VERSION 3.10)

project(PeriCom C)

# Use sane defaults
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include path for all targets
set(PERICOM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${PERICOM_INCLUDE_DIR})

# Library sources
file(GLOB PERICOM_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/library/*.c")

# Shared library -> libpericom_api.so
add_library(pericom_api SHARED ${PERICOM_LIB_SOURCES})
set_target_properties(pericom_api PROPERTIES
    OUTPUT_NAME "pericom_api"
)
target_include_directories(pericom_api PUBLIC ${PERICOM_INCLUDE_DIR})

# Static library -> libpericom_api_static.a
add_library(pericom_api_static STATIC ${PERICOM_LIB_SOURCES})
set_target_properties(pericom_api_static PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "pericom_api_static"
)
target_include_directories(pericom_api_static PUBLIC ${PERICOM_INCLUDE_DIR})

# Build each program: for every subdirectory in program/, compile *.c into an executable
file(GLOB PROGRAM_CHILDREN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/program ${CMAKE_CURRENT_SOURCE_DIR}/program/*)
foreach(child ${PROGRAM_CHILDREN})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/program/${child}")
        file(GLOB PROG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/program/${child}/*.c")
        if(PROG_SOURCES)
            message(STATUS "Adding program target: ${child}")
            add_executable(${child} ${PROG_SOURCES})
            target_include_directories(${child} PRIVATE ${PERICOM_INCLUDE_DIR})
            target_link_libraries(${child} PRIVATE pericom_api_static)
        endif()
    endif()
endforeach()

# Install rules (optional)
install(TARGETS pericom_api pericom_api_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Optionally install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
