cmake_minimum_required(VERSION 3.16)
project(demo)

add_executable(demo demo.cc)

file(COPY demo.ini DESTINATION ./)