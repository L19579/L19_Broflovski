project(L19_Broflovski)

add_executable(example example.c)
find_package(PostgreSQL REQUIRED)

target_link_libraries(example
  PRIVATE
  prompt
  broflovski
  curl
  json-c
  #libpq-fe
  ${PostgreSQL_LIBRARIES}
)
