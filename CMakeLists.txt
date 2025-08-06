# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Le decimos al sistema que, además de 'components', también busque herramientas en nuestro almacén local.
set(EXTRA_COMPONENT_DIRS components_dependencies)
add_compile_definitions(LVGL_VERSION_MAJOR=9)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(DIYTOGUETHER)