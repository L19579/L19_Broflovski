project(L19_Broflovski)

add_executable(main main.c)

target_link_libraries(main
  PRIVATE
  prompt
  broflovski
  curl
  json-c
)
