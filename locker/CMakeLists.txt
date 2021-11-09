cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(locker_lib
	${CMAKE_CURRENT_SOURCE_DIR}/screen_locker.c
	${CMAKE_CURRENT_SOURCE_DIR}/explicit_bzero.c
	)
