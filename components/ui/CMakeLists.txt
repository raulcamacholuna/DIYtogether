# Fecha: 17/08/2025 - 01:57 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Corregida la sintaxis de CMake para la expansión de variables de la lista de fuentes.
# Descripción: Se ha corregido la forma en que se pasa la variable de fuentes a idf_component_register. La sintaxis anterior causaba un error de expansión de variable, haciendo que CMake buscara un fichero inexistente '$'. La sintaxis corregida asegura la correcta expansión de la lista de ficheros fuente, resolviendo el error de compilación.

file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
    "ui_telemetry.c"
    "assets/*.c"
    "buttons/*.c"
)

idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS
        "."
        "buttons"
        "actions"
    PRIV_INCLUDE_DIRS "."
    REQUIRES
        lvgl
        bsp
        core
        screen_manager
        wifi_portal
        web_server
)
