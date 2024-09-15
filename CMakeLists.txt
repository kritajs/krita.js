cmake_minimum_required(VERSION 3.15)

project(kritajs LANGUAGES CXX)

# We don't use a "proper" installation of Qt so we need to tell CMake where our Qt directory is
list(APPEND CMAKE_PREFIX_PATH "./deps/qt")
find_package(Qt5 REQUIRED Core Widgets)
# Auto-run moc for our Qt files
set(CMAKE_AUTOMOC ON)

link_directories(
  "./deps/ultralight-sdk-1.3.0-win-x64/bin"
  "C:/Program Files/Krita (x64)/bin"
)

# Compile plugin as a shared library
add_library(plugin SHARED "./src/kritajs/kritajs.cpp")
target_include_directories(plugin PUBLIC "./src/kritajs/libkis")
target_include_directories(plugin PUBLIC "./deps/ultralight-sdk-1.3.0-win-x64/include")
target_link_libraries(plugin Qt5::Core Qt5::Widgets)
target_link_libraries(plugin AppCore Ultralight UltralightCore WebCore)
target_link_libraries(plugin libkritalibkis)

# Set library output directory and name
set_target_properties(plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../src/plugin/kritajs/bin")
set_target_properties(plugin PROPERTIES PREFIX "")
set_target_properties(plugin PROPERTIES OUTPUT_NAME "kritajs")
