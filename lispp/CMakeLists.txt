cmake_minimum_required(VERSION 3.5)
project(lispp)
set (CMAKE_CXX_STANDARD 17)


# Add other .cpp files to this library
add_library(lispp-lib
  lispp/lispp.cpp)

add_executable(lispp
  lispp/main.cpp)

target_link_libraries(lispp
  lispp-lib)

add_executable(test_lispp
  test/test_boolean.cpp
  test/test_control_flow.cpp
  test/test_eval.cpp
  test/test_integer.cpp
  test/test_lambda.cpp
  test/test_list.cpp
  test/test_symbol.cpp)
#  ../commons/catch_main.cpp
