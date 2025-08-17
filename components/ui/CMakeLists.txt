# Fecha: 17/08/2025 - 02:53 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Corregido el formato de los comentarios para que sea compatible con CMake.
# Descripción: Se ha corregido un error de compilación cambiando los comentarios de estilo C (/* */) a estilo CMake (#). Este error impedía a CMake analizar el fichero.

file(GLOB component_sources
    "*.c"
    "core/*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
    "assets/*.c"
    "buttons/*.c"
)

idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS
        "."
        "buttons"
        "actions"
        "core"
        "screens"
    PRIV_INCLUDE_DIRS "."
    REQUIRES
        lvgl
        bsp
        core
        screen_manager
        wifi_portal
        web_server
)
