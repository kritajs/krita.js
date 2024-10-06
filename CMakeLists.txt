cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(kritajs LANGUAGES CXX)

set(DEPS_PATH ${CMAKE_SOURCE_DIR}/packages/deps/installed)

# ==========================================================================================
# BUILD JS FRONTEND
# ==========================================================================================

set(FRONTEND_PATH ${CMAKE_SOURCE_DIR}/src/frontend)
set(FRONTEND_OUTPUT_PATH ${CMAKE_BINARY_DIR}/frontend)
add_custom_target(
  frontend
  WORKING_DIRECTORY ${FRONTEND_PATH}
  COMMAND npm run build -- --outdir=${FRONTEND_OUTPUT_PATH}
  COMMENT "Building krita.js frontend..."
)
file(MAKE_DIRECTORY ${FRONTEND_OUTPUT_PATH})

# ==========================================================================================
# BUILD C++ BACKEND
# ==========================================================================================

# Qt
# We don't use a "proper" installation of Qt so we need to tell CMake where our Qt directory is
list(APPEND CMAKE_PREFIX_PATH ${DEPS_PATH}/qt)
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

# Ultralight - AppCore.dll
set(ULTRALIGHT_PATH ${DEPS_PATH}/ultralight)
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
set(BACKEND_PATH ./src/backend/src)
add_library(backend SHARED
  ${BACKEND_PATH}/plugin.cpp
  ${BACKEND_PATH}/renderer.cpp
  ${BACKEND_PATH}/view.cpp
  ${BACKEND_PATH}/binding/binding.cpp
  ${BACKEND_PATH}/binding/q_object_proxy.cpp
)
target_include_directories(backend PRIVATE ${BACKEND_PATH})
target_include_directories(backend PRIVATE ${BACKEND_PATH}/binding)
add_dependencies(backend frontend)
target_link_libraries(backend PRIVATE Qt5::Core Qt5::Widgets)
target_link_libraries(backend PRIVATE libkis)
target_link_libraries(backend PRIVATE AppCore Ultralight UltralightCore WebCore)
# Set output directory and file name
set_target_properties(backend PROPERTIES OUTPUT_NAME kritajs)

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

# Copy frontend files
install(DIRECTORY ${FRONTEND_PATH}/public/ DESTINATION ${INSTALL_PATH}/assets)
install(DIRECTORY ${FRONTEND_OUTPUT_PATH}/ DESTINATION ${INSTALL_PATH}/assets)

# Copy required DLLs
install(TARGETS backend RUNTIME DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS AppCore DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS Ultralight DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS UltralightCore DESTINATION ${INSTALL_PATH}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS WebCore DESTINATION ${INSTALL_PATH}/bin)
