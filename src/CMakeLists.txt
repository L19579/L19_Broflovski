project(L19_Broflovski)

add_library(amm_keys amm_keys.c)
add_library(broflovski broflovski.c)

target_include_directories(amm_keys
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_include_directories(broflovski
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
