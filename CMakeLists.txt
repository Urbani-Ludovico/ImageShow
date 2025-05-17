cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Werror)

project(ImageShow C)
add_executable(${PROJECT_NAME}
    src/main.c
    src/files.c
    include/files.h
)

include_directories(include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
if(GTK4_FOUND)
    message(STATUS "GTK4 found:")
    message(STATUS "  Includes: ${GTK4_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${GTK4_LIBRARIES}")
    message(STATUS "  Version: ${GTK4_VERSION}")
else()
    message(FATAL_ERROR "GTK4 not found. Please install GTK4 development packages.")
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GTK4_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${GTK4_CFLAGS_OTHER})

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})