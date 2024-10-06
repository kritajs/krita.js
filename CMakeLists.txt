cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(kritajs LANGUAGES CXX)

set(DEPS_PATH ${CMAKE_SOURCE_DIR}/deps)

# ==========================================================================================
# BINDGEN
# ==========================================================================================

# set(BINDGEN_ROOT_PATH ${CMAKE_SOURCE_DIR}/tools/bindgen)
# set(BINDGEN_COMMAND ${BINDGEN_ROOT_PATH}/ENV/Scripts/python ${BINDGEN_ROOT_PATH}/src/bindgen.py)
# set(BINDGEN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bindgen)

# # Generate libkis headers
# set(BINDGEN_HEADERS_OUTPUT_PATH ${BINDGEN_OUTPUT_PATH}/headers)
# add_custom_target(
#   bindgen_headers
#   COMMAND ${BINDGEN_COMMAND} ${DEPS_PATH}/krita -o ${BINDGEN_HEADERS_OUTPUT_PATH} -g header
# )
# file(MAKE_DIRECTORY ${BINDGEN_HEADERS_OUTPUT_PATH})

# ==========================================================================================
# BUILD PLUGIN MANAGER
# ==========================================================================================

set(PLUGIN_MANAGER_PATH ${CMAKE_SOURCE_DIR}/src/plugin_manager)
set(PLUGIN_MANAGER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/plugin_manager)
add_custom_target(
  plugin_manager
  WORKING_DIRECTORY ${PLUGIN_MANAGER_PATH}
  COMMAND npm run build -- --outdir=${PLUGIN_MANAGER_OUTPUT_PATH}
  COMMENT "Building krita.js Plugin Manager..."
)
file(MAKE_DIRECTORY ${PLUGIN_MANAGER_OUTPUT_PATH})

# ==========================================================================================
# BUILD C++ PLUGIN
# ==========================================================================================

# Qt
# We don't use a "proper" installation of Qt so we need to tell CMake where our Qt directory is
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/deps/qt)
find_package(Qt5 REQUIRED Core Widgets)
# Auto-run moc for our Qt files
set(CMAKE_AUTOMOC ON)

# Krita - libkritalibkis.dll
set(KRITA_INSTALL_PATH "C:/Program Files/Krita (x64)")
add_library(libkis SHARED IMPORTED)
add_dependencies(libkis bindgen_headers)
set_target_properties(libkis PROPERTIES
  IMPORTED_LOCATION ${KRITA_INSTALL_PATH}/bin/libkritalibkis.dll
  # libkis doesn't have a .lib file so we just point it to anything.
  # It's messy but it works ¯\_(ツ)_/¯
  IMPORTED_IMPLIB ${KRITA_INSTALL_PATH}/bin/libkritalibkis.dll
)
# target_include_directories(libkis INTERFACE ${BINDGEN_HEADERS_OUTPUT_PATH})

# Ultralight - AppCore.dll
set(ULTRALIGHT_PATH ${DEPS_PATH}/ultralight-sdk-1.3.0-win-x64)
add_library(AppCore SHARED IMPORTED)
set_target_properties(AppCore PROPERTIES
  IMPORTED_LOCATION ${ULTRALIGHT_PATH}/bin/AppCore.dll
  IMPORTED_IMPLIB ${ULTRALIGHT_PATH}/lib/AppCore.lib
)
target_include_directories(AppCore INTERFACE ${ULTRALIGHT_PATH}/include)

# Ultralight - Ultralight.dll
add_library(Ultralight SHARED IMPORTED)
set_target_properties(Ultralight PROPERTIES
  IMPORTED_LOCATION ${ULTRALIGHT_PATH}/bin/Ultralight.dll
  IMPORTED_IMPLIB ${ULTRALIGHT_PATH}/lib/Ultralight.lib
)
target_include_directories(Ultralight INTERFACE ${ULTRALIGHT_PATH}/include)

# Ultralight - UltralightCore.dll
add_library(UltralightCore SHARED IMPORTED)
set_target_properties(UltralightCore PROPERTIES
  IMPORTED_LOCATION ${ULTRALIGHT_PATH}/bin/UltralightCore.dll
  IMPORTED_IMPLIB ${ULTRALIGHT_PATH}/lib/UltralightCore.lib
)
target_include_directories(UltralightCore INTERFACE ${ULTRALIGHT_PATH}/include)

# Ultralight - WebCore.dll
add_library(WebCore SHARED IMPORTED)
set_target_properties(WebCore PROPERTIES
  IMPORTED_LOCATION ${ULTRALIGHT_PATH}/bin/WebCore.dll
  IMPORTED_IMPLIB ${ULTRALIGHT_PATH}/lib/WebCore.lib
)
target_include_directories(WebCore INTERFACE ${ULTRALIGHT_PATH}/include)

# Compile krita.js plugin as a shared library
set(PLUGIN_PATH ./src/plugin/src)
add_library(plugin SHARED
  ${PLUGIN_PATH}/plugin.cpp
  ${PLUGIN_PATH}/renderer.cpp
  ${PLUGIN_PATH}/view.cpp
  ${PLUGIN_PATH}/binding/binding.cpp
  ${PLUGIN_PATH}/binding/q_object_proxy.cpp
)
target_include_directories(plugin PRIVATE ${PLUGIN_PATH})
target_include_directories(plugin PRIVATE ${PLUGIN_PATH}/binding)
add_dependencies(plugin plugin_manager)
target_link_libraries(plugin PRIVATE Qt5::Core Qt5::Widgets)
target_link_libraries(plugin PRIVATE libkis)
target_link_libraries(plugin PRIVATE AppCore Ultralight UltralightCore WebCore)
# Set output directory and file name
set_target_properties(plugin PROPERTIES OUTPUT_NAME kritajs)

# ==========================================================================================
# PACKAGING
# ==========================================================================================
# Directory where CMake should package the plugin after building
set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/build_package/kritajs)

# Copy Python wrapper plugin
install(DIRECTORY ./src/wrapper/src/ DESTINATION ${INSTALL_PATH})
install(FILES ./src/wrapper/kritajs.desktop DESTINATION ${INSTALL_PATH}/..)

# Copy Ultralight resources. Ultralight needs these to run.
install(DIRECTORY ${ULTRALIGHT_PATH}/resources DESTINATION ${INSTALL_PATH}/assets)

# Copy Ultralight inspector
# install(DIRECTORY ${ULTRALIGHT_PATH}/inspector DESTINATION ${INSTALL_PATH}/assets)

# Copy plugin manager files
install(DIRECTORY ${PLUGIN_MANAGER_PATH}/public/ DESTINATION ${INSTALL_PATH}/assets)
install(DIRECTORY ${PLUGIN_MANAGER_OUTPUT_PATH}/ DESTINATION ${INSTALL_PATH}/assets)

# Copy required DLLs
install(TARGETS plugin RUNTIME DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS AppCore DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS Ultralight DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS UltralightCore DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS WebCore DESTINATION ${INSTALL_PATH}/bin)
