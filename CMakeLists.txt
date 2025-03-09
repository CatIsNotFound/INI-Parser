cmake_minimum_required(VERSION 3.10)
project(MyParser CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(STATIC INI-Parser.h)