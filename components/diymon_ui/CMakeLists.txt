# Fichero: ./components/diymon_ui/CMakeLists.txt
# Fecha: 11/08/2025 - 16:15
# Último cambio: Versión final y verificada.
# Descripción: Se definen las dependencias del componente de la UI.

file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
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