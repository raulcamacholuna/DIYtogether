# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\CMakeLists.txt
# Fecha: $timestamp
# Último cambio: Eliminada la referencia al directorio 'assets' que ya no existe.
# Descripción: CMakeLists para el componente de UI. Se elimina `PRIV_INCLUDE_DIRS "assets"` para resolver el error de compilación, ya que el directorio 'assets' ha sido borrado.

file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
    "ui_telemetry.c"
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
