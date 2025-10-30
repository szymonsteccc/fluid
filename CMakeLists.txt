cmake_minimum_required(VERSION 3.10)
project(Fluid C)

set(CMAKE_C_STANDARD 11)

set(SOURCES
  src/simulation.c
  src/app.c
  src/particle.c
  src/physics.c
)
add_executable(fluid ${SOURCES})

target_include_directories(fluid PRIVATE ${CMAKE_SOURCE_DIR}/src /usr/local/Cellar/sdl3/3.2.24/include)
target_include_directories(fluid PRIVATE /usr/local/Cellar/sdl3/3.2.24/include)

target_link_directories(fluid PRIVATE /usr/local/Cellar/sdl3/3.2.24/lib)
target_link_libraries(fluid PRIVATE SDL3)