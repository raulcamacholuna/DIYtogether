# Fecha: 20/08/2025 - 04:38
# Fichero: ./CMakeLists.txt
# Último cambio: Limpiado para eliminar directivas de enlazado incorrectas.
# Descripción: Fichero de compilación raíz. Se han eliminado todos los intentos de 'target_link_libraries'. La solución correcta al problema de enlazado reside en el CMakeLists.txt del componente 'main'.

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS components_dependencies)

add_compile_definitions(
    LVGL_VERSION_MAJOR=9
    LV_LVGL_H_INCLUDE_SIMPLE
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(DIYTOGETHER)
