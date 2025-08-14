# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\CMakeLists.txt
# Fecha: $timestamp
# Último cambio: Actualizado para incluir `ui_config_screen.c` en la compilación.
# Descripción: El `file(GLOB "*.c")` existente se encarga de encontrar y compilar automáticamente el nuevo fichero `ui_config_screen.c`. Se regenera el fichero para confirmar que este nuevo fuente se añade al componente.

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
)
