cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Werror)

project(ImageShow C)
add_executable(ImageShow
        src/main.c
)

include_directories(include)

target_compile_options(ImageShow PRIVATE ${COMPILE_OPTIONS})