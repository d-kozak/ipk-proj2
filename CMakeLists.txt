cmake_minimum_required(VERSION 3.3)
project(ipk_proj2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/david/Documents/studium/4.semestr/IPK/ipk-proj2/binary")

add_executable(client client.cpp client.h protokol_parser.cpp protokol_parser.h socket_handler.cpp socket_handler.h )
add_executable(server server.cpp server.h protokol_parser.cpp protokol_parser.h socket_handler.cpp socket_handler.h )