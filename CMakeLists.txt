cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(CXX)

add_link_options(
	-pedantic 
	-Wall 
	-Wextra
)

project(screen_lock)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
include_directories(${X11_Xrandr_INCLUDE_PATH})
include_directories(${X11_Xext_INCLUDE_PATH})

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
	X11::Xext
	X11::Xrandr
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
