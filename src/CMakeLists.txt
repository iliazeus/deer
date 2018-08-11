set(SOURCES
  matrix.h
  transform.cc
  transform.h
  vector.h
)
add_library(deer ${SOURCES})

add_executable(main main.cc)
target_link_libraries(main deer)
