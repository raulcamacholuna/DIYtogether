# Fichero: ./components/diymon_ui/CMakeLists.txt
# Fecha: 13/08/2025 - 12:08 
# Último cambio: Corregida la sintaxis de la variable de CMake para las fuentes.
# Descripción: Se corrige el error de sintaxis en idf_component_register que impedía
#              la correcta expansión de la lista de ficheros fuente, solucionando el fallo
#              de compilación.

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
