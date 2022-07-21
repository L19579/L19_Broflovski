project(L19_Broflovski)

add_library(prompt prompt.c)
add_library(broflovski broflovski.c)

target_include_directories(prompt
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_include_directories(broflovski
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
