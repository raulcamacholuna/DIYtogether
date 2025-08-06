# CMakeLists.txt para el componente 'diymon_ui'.
# Versión automatizada para gestionar fácilmente los archivos generados por EEZ Studio.

# 1. Buscar automáticamente todos los archivos fuente (.c) en este directorio.
#    La función file(GLOB ...) crea una lista de todos los archivos que terminan en .c
#    y la guarda en la variable 'component_sources'.
set(component_sources
    "actions.c"
    "screens.c"
    "styles.c"
    "ui.c"
)

# 2. Registrar el componente con la lista de archivos que hemos encontrado.
idf_component_register(
    # SRCS: Usa la variable que hemos creado con todos los archivos .c
    SRCS ${component_sources}

    # INCLUDE_DIRS: Permite que otros componentes puedan hacer #include "fichero.h" de este.
    INCLUDE_DIRS "."

    # PRIV_INCLUDE_DIRS: Directorios de inclusión privados para este componente.
    PRIV_INCLUDE_DIRS "."
    
    # REQUIRES: Lista de otros componentes de los que 'diymon_ui' depende.
    # - lvgl: Necesario porque toda la UI está basada en él.
    # - NOTA: En ESP-IDF v5.x, 'esp_log' se incluye automáticamente y NO debe listarse aquí.
    REQUIRES 
        "lvgl__lvgl"
        "diymon_core"
)
