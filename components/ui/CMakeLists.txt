# Fecha: 16/08/2025 - 09:24 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Corregidas las dependencias de componentes para que coincidan con los nuevos nombres de directorio.
# Descripción: Se ha actualizado la lista `REQUIRES` para usar los nombres de componente cortos ('bsp', 'core') en lugar de los antiguos ('diymon_bsp', 'diymon_core'). Esto resuelve el error de compilación de CMake "Failed to resolve component 'diymon_bsp'".

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
