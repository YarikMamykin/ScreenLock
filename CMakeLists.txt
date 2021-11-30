cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-g -pedantic -Wall -Os")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(screen_lock)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

include_directories(utils)
include_directories(config)
include_directories(arg)
include_directories(user)
include_directories(helpers)
include_directories(window)
include_directories(input_handling)

add_subdirectory(user)
add_subdirectory(helpers)
add_subdirectory(window)
add_subdirectory(input_handling)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} 
	user_lib 
	helpers_lib 
	locking_window_lib 
	password_input_handler_lib
	c
	crypt
	Xext
	Xrandr
)

set(CMAKE_INSTALL_PREFIX /usr/bin)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
