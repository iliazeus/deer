cmake_minimum_required(VERSION 3.0)


project(deer
  VERSION 0.0.1
  LANGUAGES CXX
)


include_directories(src)


set(CMAKE_BUILD_TYPE Debug)


set(CMAKE_CXX_FLAGS "-std=c++17")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


enable_testing()


add_subdirectory(src)
add_subdirectory(tests)

