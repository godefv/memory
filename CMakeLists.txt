cmake_minimum_required(VERSION 3.8)
project(godefv_memory CXX)

add_subdirectory(extern/godefv/error_checking)
include_directories("extern")
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

godefv_add_module(memory)
target_compile_features(godefv_memory INTERFACE cxx_std_14)

enable_testing()
godefv_add_test(memory object_pool godefv_error_checking)
