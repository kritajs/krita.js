cmake_minimum_required(VERSION 3.10)
project(kritajs)

# Compile loader.cpp as a dynamic library
add_library(loader SHARED src/loader/loader.cpp)

# Include header files
set(DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)
target_include_directories(loader PRIVATE ${DEPS_DIR}/qtbase-5.15.2-mingw81_64/include/)
target_include_directories(loader PRIVATE ${DEPS_DIR}/qtbase-5.15.2-mingw81_64/include/QtCore)

# Link Krita libraries
target_link_directories(loader PRIVATE "C:\\Program Files\\Krita (x64)\\bin")
target_link_libraries(loader Qt5Core)
