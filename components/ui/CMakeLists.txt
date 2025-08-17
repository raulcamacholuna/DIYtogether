# Fecha: 17/08/2025 - 02:27 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Añadido el subdirectorio 'core' a la lista de fuentes y directorios de inclusión para reflejar la nueva estructura del componente.
# Descripción: Fichero de compilación para el componente UI. Se ha actualizado para incluir los ficheros fuente y las cabeceras del nuevo subdirectorio 'core', que contiene la lógica principal de la UI (inicialización, gestión de estado y telemetría).

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
    PRIV_INCLUDE_DIRS "."
    REQUIRES
        lvgl
        bsp
        core
        screen_manager
        wifi_portal
        web_server
)
