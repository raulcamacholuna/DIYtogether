# Fecha: 25/08/2025 - 12:00
# Fichero: ./CMakeLists.txt
# Último cambio: AÑADIDA LA SOLUCIÓN DEFINITIVA AL PROBLEMA DE ENLAZADO DE ZIGBEE.
# Descripción: El error 'Failed to resolve component 'espressif__esp-zboss-lib'' es causado
#              porque el sistema de build ignora las directivas 'override_path' de los ficheros .yml
#              para dependencias anidadas.
# Solución: Se establece la variable global 'COMPONENT_OVERRIDE_PATH'. Esta directiva de CMake
#           fuerza al sistema de build a usar las copias locales de las librerías Zigbee,
#           anulando cualquier otra búsqueda y resolviendo el error de configuración de forma definitiva.

cmake_minimum_required(VERSION 3.16)

# Forzar al sistema de build a usar nuestras copias locales de las librerías Zigbee.
# Esta es la directiva que soluciona el problema.
set(COMPONENT_OVERRIDE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/components_dependencies/esp-zigbee-lib
    ${CMAKE_CURRENT_LIST_DIR}/components_dependencies/esp-zboss-lib
)

# El list(APPEND EXTRA_COMPONENT_DIRS...) sigue siendo necesario para que los componentes
# que NO están en el registro (como los nuestros) sean descubiertos.
list(APPEND EXTRA_COMPONENT_DIRS
    components_dependencies
)

add_compile_definitions(
    LVGL_VERSION_MAJOR=9
    LV_LVGL_H_INCLUDE_SIMPLE
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(DIYTOGETHER)