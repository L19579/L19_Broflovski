project(L19_Broflovski)

add_library(toml toml.c)

target_include_directories(toml
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/tomlc
)
