cmake_minimum_required(VERSION 3.14)
project(L19_Broflovski)

#add_subdirectory(thirdparty)
add_subdirectory(include)
add_subdirectory(src)

option(BUILD_TARGET "build targets" ON)
if (BUILD_TARGET)
  message(STATUS "building target..")
  add_subdirectory(target)
endif()
