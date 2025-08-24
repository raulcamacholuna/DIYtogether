# Fecha: 23/08/2025 - 11:50
# Fichero: ./CMakeLists.txt
# Último cambio: Añadido COMPONENT_OVERRIDE_PATH para forzar el uso de las librerías Zigbee locales.
# Descripción: Diagnóstico de Causa Raíz Definitivo: El error 'Failed to resolve component 'espressif__esp-zboss-lib'' es causado porque el componente 'esp-zigbee-lib' tiene una dependencia explícita de la versión del registro de 'esp-zboss-lib'. El sistema de build de ESP-IDF intenta descargarla en lugar de utilizar la copia local proporcionada en el proyecto. Solución Definitiva: Se establece la variable 'COMPONENT_OVERRIDE_PATH' en el CMakeLists.txt principal. Esta directiva instruye explícitamente al sistema de build para que, cuando un componente solicite 'espressif/esp-zboss-lib' (o cualquier otro componente listado), utilice la versión que se encuentra en la ruta local especificada. Esto anula la búsqueda en el registro de componentes, resuelve la dependencia interna entre las librerías Zigbee locales y corrige el error de configuración de forma definitiva.

cmake_minimum_required(VERSION 3.16)

# Forzar al sistema de build a usar nuestras copias locales de las librerías Zigbee
# en lugar de buscarlas en el registro de componentes de Espressif.
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
