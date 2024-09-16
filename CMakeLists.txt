cmake_minimum_required(VERSION 3.15)

project(kritajs LANGUAGES CXX)

# ==========================================================================================
# COMPILING
# ==========================================================================================

# Directory where CMake should package the plugin after building
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build_package")
set(DEPS_PATH "${CMAKE_SOURCE_DIR}/deps")
set(ULTRALIGHT_PATH "${DEPS_PATH}/ultralight-sdk-1.3.0-win-x64")

# Qt
# We don't use a "proper" installation of Qt so we need to tell CMake where our Qt directory is
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/deps/qt")
find_package(Qt5 REQUIRED Core Widgets)
# Auto-run moc for our Qt files
set(CMAKE_AUTOMOC ON)

# Krita - libkritalibkis.dll
set(KRITA_INSTALL_PATH "C:/Program Files/Krita (x64)")
add_library(libkis SHARED IMPORTED)
set_target_properties(libkis PROPERTIES
  IMPORTED_LOCATION "${KRITA_INSTALL_PATH}/bin/libkritalibkis.dll"
  IMPORTED_IMPLIB "${KRITA_INSTALL_PATH}/bin/libkritalibkis.dll"
)
target_include_directories(libkis INTERFACE "./src/plugin/libkis")

# Ultralight - AppCore.dll
add_library(AppCore SHARED IMPORTED)
set_target_properties(AppCore PROPERTIES
  IMPORTED_LOCATION "${ULTRALIGHT_PATH}/bin/AppCore.dll"
  IMPORTED_IMPLIB "${ULTRALIGHT_PATH}/lib/AppCore.lib"
)
target_include_directories(AppCore INTERFACE "${ULTRALIGHT_PATH}/include")

# Ultralight - Ultralight.dll
add_library(Ultralight SHARED IMPORTED)
set_target_properties(Ultralight PROPERTIES
  IMPORTED_LOCATION "${ULTRALIGHT_PATH}/bin/Ultralight.dll"
  IMPORTED_IMPLIB "${ULTRALIGHT_PATH}/lib/Ultralight.lib"
)
target_include_directories(Ultralight INTERFACE "${ULTRALIGHT_PATH}/include")

# Ultralight - UltralightCore.dll
add_library(UltralightCore SHARED IMPORTED)
set_target_properties(UltralightCore PROPERTIES
  IMPORTED_LOCATION "${ULTRALIGHT_PATH}/bin/UltralightCore.dll"
  IMPORTED_IMPLIB "${ULTRALIGHT_PATH}/lib/UltralightCore.lib"
)
target_include_directories(UltralightCore INTERFACE "${ULTRALIGHT_PATH}/include")

# Ultralight - WebCore.dll
add_library(WebCore SHARED IMPORTED)
set_target_properties(WebCore PROPERTIES
  IMPORTED_LOCATION "${ULTRALIGHT_PATH}/bin/WebCore.dll"
  IMPORTED_IMPLIB "${ULTRALIGHT_PATH}/lib/WebCore.lib"
)
target_include_directories(WebCore INTERFACE "${ULTRALIGHT_PATH}/include")

# Compile krita.js plugin as a shared library
add_library(plugin SHARED "./src/plugin/plugin.cpp")
target_include_directories(plugin PRIVATE "./src/plugin/libkis")
target_link_libraries(plugin PRIVATE Qt5::Core Qt5::Widgets)
target_link_libraries(plugin PRIVATE libkis)
target_link_libraries(plugin PRIVATE AppCore Ultralight UltralightCore WebCore)
# Set output directory and file name
set_target_properties(plugin PROPERTIES OUTPUT_NAME "kritajs")

# ==========================================================================================
# INSTALLING
# ==========================================================================================
set(INSTALL_DIR "./kritajs")

# Copy Python wrapper plugin
install(DIRECTORY "./src/wrapper/src/" DESTINATION "${INSTALL_DIR}")
install(FILES "./src/wrapper/kritajs.desktop" DESTINATION "${INSTALL_DIR}/..")

# Copy Ultralight resources. Ultralight needs these to run.
install(DIRECTORY "${ULTRALIGHT_PATH}/resources" DESTINATION "${INSTALL_DIR}")

# Copy Ultralight sample assets. Just for testing purposes so that we have something to display.
install(DIRECTORY "${ULTRALIGHT_PATH}/samples/Sample 2 - Basic App/assets" DESTINATION "${INSTALL_DIR}")

# Copy required DLLs
install(TARGETS plugin RUNTIME DESTINATION "${INSTALL_DIR}/bin")
install(IMPORTED_RUNTIME_ARTIFACTS AppCore DESTINATION "${INSTALL_DIR}/bin")
install(IMPORTED_RUNTIME_ARTIFACTS Ultralight DESTINATION "${INSTALL_DIR}/bin")
install(IMPORTED_RUNTIME_ARTIFACTS UltralightCore DESTINATION "${INSTALL_DIR}/bin")
install(IMPORTED_RUNTIME_ARTIFACTS WebCore DESTINATION "${INSTALL_DIR}/bin")
