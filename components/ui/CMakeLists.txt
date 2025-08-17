# Fecha: 17/08/2025 - 05:53 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Modificado para compilar automáticamente TODOS los ficheros .c de assets/images/.
# Descripción: El problema de los botones invisibles (ruido) se debía a que sus ficheros C de definición de imagen (ej. asset_btn_1.c) no se estaban incluyendo en la compilación, causando errores de enlazador. Esta modificación utiliza `file(GLOB ...)` para encontrar y compilar automáticamente todos los ficheros .c del directorio `assets/images/`. Esto elimina la necesidad de listarlos manualmente, resuelve el error de 'undefined reference' para el botón 1 y asegura que todos los iconos se enlacen y rendericen correctamente.

file(GLOB component_sources
    "*.c"
    "core/*.c"
    "screens/*.c"
    "actions/*.c"
    "assets/styles/*.c"
    "buttons/*.c"
)

# [CORRECCIÓN] Buscar y añadir automáticamente TODOS los ficheros .c del directorio de imágenes.
# Esto asegura que BG.c, BG_config.c y todos los asset_btn_*.c se compilen y enlacen correctamente.
file(GLOB asset_sources "assets/images/*.c")
list(APPEND component_sources ${asset_sources})

idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS
        "."
        "buttons"
        "actions"
        "core"
        "screens"
        "assets/styles"
        "assets/fonts"
        "assets/images"
    PRIV_INCLUDE_DIRS "."
    REQUIRES
        lvgl
        bsp
        core
        screen_manager
        wifi_portal
        web_server
)
