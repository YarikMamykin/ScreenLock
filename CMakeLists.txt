cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "/usr/bin/cc")
set(CMAKE_C_FLAGS "-g -std=c99 -pedantic -Wall -Os -D_DEFAULT_SOURCE -DHAVE_SHADOW_H -lc -lcrypt -lXext -lXrandr")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ScreenLocker)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

include_directories(utils)
include_directories(config)
include_directories(arg)
include_directories(locker)
include_directories(user)
include_directories(helpers)

add_subdirectory(locker)
add_subdirectory(user)
add_subdirectory(helpers)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} locker_lib user_lib helpers_lib)
