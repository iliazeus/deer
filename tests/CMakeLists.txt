set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(CMAKE_POLICY_DEFAULT_CMP0048 OLD)
add_subdirectory(../gtest ${CMAKE_BINARY_DIR}/gtest)

set(SOURCES
  geometry.cc
  matrix.cc
  spectrum.cc
  transform.cc
  vector.cc
)

add_executable(unit_tests ${SOURCES})
target_link_libraries(unit_tests
  gtest gtest_main
  deer
)
add_test(NAME unit_tests COMMAND unit_tests)

