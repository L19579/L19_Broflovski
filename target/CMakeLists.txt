project(L19_Broflovski)

add_executable(main main.c)
find_package(PostgreSQL REQUIRED)

target_link_libraries(main
  PRIVATE
  amm_keys
  broflovski
  curl
  json-c
  toml
  #libpq-fe
  ${PostgreSQL_LIBRARIES}
)
