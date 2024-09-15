cmake_minimum_required(VERSION 3.15)

project(kritajs LANGUAGES CXX)

# We don't use a "proper" installation of Qt so we need to tell CMake where our Qt directory is
set(CMAKE_PREFIX_PATH "deps/qt")
find_package(Qt5 REQUIRED Core)
# Auto-run moc for our Qt files
set(CMAKE_AUTOMOC ON)

link_directories(deps/kcoreaddons/bin)

# Compile plugin as a shared library
add_library(plugin SHARED src/plugin.cpp)
target_include_directories(plugin PUBLIC deps/kcoreaddons/include/KF5/KCoreAddons)
target_link_libraries(plugin Qt5::Core)
target_link_libraries(plugin KF5CoreAddons)

# Name output file "kritajs" instead of "libplugin"
set_target_properties(plugin PROPERTIES PREFIX "")
set_target_properties(plugin PROPERTIES OUTPUT_NAME "kritajs")
