cmake_minimum_required(VERSION 3.25)
project(db_in_one_hour)

set(CMAKE_CXX_STANDARD 17)

add_executable(db_in_one_hour main.cpp operator.h virtualStructure.h db.h)
