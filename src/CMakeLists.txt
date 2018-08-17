set(SOURCES
  file_formats/tga.cc
  file_formats/tga.h
  geometry.cc
  geometry.h
  matrix.h
  optics.h
  renderer.h
  renderer.cc
  scene.h
  spectrum.cc
  spectrum.h
  transform.cc
  transform.h
  vector.h
)
add_library(deer ${SOURCES})

add_executable(main main.cc)
target_link_libraries(main deer)
