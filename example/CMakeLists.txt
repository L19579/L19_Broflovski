project(L19_Broflovski)

add_executable(example example.c)
find_package(PostgreSQL REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(example
  PRIVATE
  amm_keys
  broflovski
  curl
  json-c
  toml
  ${PostgreSQL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)
