project(L19_Broflovski)

add_library(prompt prompt.c)

target_include_directories(prompt
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
