cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(password_input_handler_lib ${CMAKE_CURRENT_SOURCE_DIR}/password_input_handler.c)
