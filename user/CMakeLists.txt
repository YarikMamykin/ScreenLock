cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(user_lib ${CMAKE_CURRENT_SOURCE_DIR}/user_data.c)
