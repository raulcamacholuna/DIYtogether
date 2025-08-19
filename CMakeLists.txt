# Fecha: 17/08/2025 - 04:03 
# Fichero: ./CMakeLists.txt
# Último cambio: Reestructurados y mejorados los comentarios para mayor claridad.
# Descripción: Fichero raíz de configuración de CMake para el proyecto. Define la versión mínima de CMake,
#              el nombre del proyecto, y rutas adicionales para componentes de terceros. El error 'Permission denied'
#              del sistema de compilación (Ninja) no se origina en este fichero, sino en un problema externo
#              como un antivirus o permisos del sistema de ficheros. Se mejora este fichero como acción proactiva.

# Versión mínima de CMake requerida por ESP-IDF.
cmake_minimum_required(VERSION 3.16)

# Añade el directorio 'components_dependencies' a la ruta de búsqueda de componentes.
# Esto permite que el sistema de compilación encuentre librerías de terceros (ej. esp_lvgl_port)
# que no forman parte de los componentes estándar ni del propio proyecto.
set(EXTRA_COMPONENT_DIRS components_dependencies)

# Define una macro de preprocesador global para todo el proyecto.
# En este caso, se usa para informar a los componentes que se está utilizando LVGL v9.
add_compile_definitions(
    LVGL_VERSION_MAJOR=9
    LV_LVGL_H_INCLUDE_SIMPLE
)

# Incluye el script principal de ESP-IDF que gestiona toda la lógica de compilación de proyectos.
# Esta línea es fundamental y debe ir después de las configuraciones iniciales.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Declara el nombre del proyecto.
project(DIYTOGETHER)
