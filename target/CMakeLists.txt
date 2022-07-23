project(L19_Broflovski)

add_executable(main main.c)
find_package(PostgreSQL REQUIRED)
# Add include directories to your target. PRIVATE is useful with multi-target projects - see documentation of target_include_directories for more info
#target_include_directories(L19_Broflovski PRIVATE ${PostgreSQL_INCLUDE_DIRS})

# Add libraries to link your target againts. Again, PRIVATE is important for multi-target projects
target_link_libraries(main
  PRIVATE
  prompt
  broflovski
  curl
  json-c
  #libpq-fe
  ${PostgreSQL_LIBRARIES}
)
