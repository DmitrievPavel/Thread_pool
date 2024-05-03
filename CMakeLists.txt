
cmake_minimum_required (VERSION 3.8)

project ("threadpool")

add_executable (threadpool "threadpool.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET threadpool PROPERTY CXX_STANDARD 20)
endif()


