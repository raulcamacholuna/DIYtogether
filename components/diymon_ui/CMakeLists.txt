# Fichero: ./components/diymon_ui/CMakeLists.txt
# Fecha: 13/08/2025 - 11:27 
# Último cambio: Añadido images.c a la lista de fuentes.
# Descripción: Fichero de compilación de la UI. Se añade el nuevo fichero
#              'images.c' a la lista de ficheros fuente para que sea
#              compilado y enlazado, solucionando el error 'undefined reference'.

file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
    "ui_telemetry.c"
)

# Se añade explícitamente el nuevo fichero a la lista de fuentes
list(APPEND component_sources "images.c")

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
