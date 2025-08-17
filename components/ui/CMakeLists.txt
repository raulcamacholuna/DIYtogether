# Fecha: 17/08/2025 - 01:17 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Añadido el subdirectorio 'buttons' a las fuentes y a las rutas de inclusión para soportar la refactorización de los botones.
# Descripción: Fichero de compilación del componente de la UI. Se ha actualizado para incluir todos los ficheros .c del nuevo subdirectorio 'buttons' en la compilación y para añadir 'buttons' a las rutas de inclusión, permitiendo que el resto del componente pueda acceder a las nuevas cabeceras de los botones modulares.

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
    PRIV_INCLUDE_DIRS "."
    REQUIRES 
        lvgl
        bsp
        core
        screen_manager
        wifi_portal
        web_server 
)
