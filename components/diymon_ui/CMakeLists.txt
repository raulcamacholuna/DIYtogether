# Fecha: 16/08/2025 - 08:09 
# Fichero: components/diymon_ui/CMakeLists.txt
# Último cambio: Añadida la dependencia 'web_server' para resolver el error de compilación.
# Descripción: El componente diymon_ui ahora depende de web_server para poder iniciar el servidor de configuración. Se ha añadido a la lista REQUIRES para que el compilador encuentre 'web_server.h'.

file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
    "ui_telemetry.c"
    "assets/*.c"
)

idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS "."
    PRIV_INCLUDE_DIRS "."
    REQUIRES 
        lvgl
        diymon_bsp
        diymon_core
        screen_manager
        wifi_portal
        web_server 
)
