cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)
set(SUPPORTED_LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17) # Krita will not compile with versions lower than 17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(GenerateExportHeader)
project(kritajs LANGUAGES ${SUPPORTED_LANGUAGES})

# Compile loader.cpp as a dynamic library
add_library(loader SHARED src/loader/loader.cpp)

# Krita uses a bunch of generated header files during the build process. We mimic that here.
# Macro to easily generate export headers
macro(generate_export libname)
  add_library(${libname} src/loader/dummy.cpp) # Libraries require at least one file so we use an empty file
  generate_export_header(${libname})
endmacro()
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Include the build folder so that the compiler can find the generated headers
generate_export(ki18n)
generate_export(kritacommand)
generate_export(kritaglobal)
generate_export(kritaimage)
generate_export(kritalibkis)
generate_export(kritapigment)
generate_export(kritaresources)
generate_export(kritaui)
generate_export(kritawidgets)
generate_export(kritawidgetutils)

# Include header files
set(DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)
target_include_directories(loader PRIVATE ${DEPS_DIR}/boost_1_80_0)
target_include_directories(loader PRIVATE ${DEPS_DIR}/eigen-3.4.0)
target_include_directories(loader PRIVATE ${DEPS_DIR}/ki18n/src/i18n)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/command)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/global)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/image)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/image/filter)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/libkis)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/pigment)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/pigment/resources)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/resources)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/ui/tool)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/widgets)
target_include_directories(loader PRIVATE ${DEPS_DIR}/krita/libs/widgetutils)
target_include_directories(loader PRIVATE ${DEPS_DIR}/qtbase-5.15.2-mingw81_64/include/)
target_include_directories(loader PRIVATE ${DEPS_DIR}/qtbase-5.15.2-mingw81_64/include/QtCore)
target_include_directories(loader PRIVATE ${DEPS_DIR}/qtbase-5.15.2-mingw81_64/include/QtGui)
target_include_directories(loader PRIVATE ${DEPS_DIR}/qtbase-5.15.2-mingw81_64/include/QtWidgets)
target_include_directories(loader PRIVATE ${DEPS_DIR}/qtbase-5.15.2-mingw81_64/include/QtXml)
target_include_directories(loader PRIVATE src/loader)

# Link Krita libraries
target_link_directories(loader PRIVATE "C:\\Program Files\\Krita (x64)\\bin")
target_link_libraries(loader Qt5Core)
