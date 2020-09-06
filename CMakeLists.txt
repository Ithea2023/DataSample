cmake_minimum_required (VERSION 3.6)

project(DataSample)
set(CMAKE_CXX_STANDARD 17)

# Add sub directories
add_subdirectory(main)
# add_subdirectory(bbr)
add_subdirectory(checks)
add_subdirectory(unit_base)
add_subdirectory(units)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)