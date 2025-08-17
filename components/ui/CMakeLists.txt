# Fecha: 17/08/2025 - 09:43 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Añadido el subdirectorio 'telemetry' a las rutas de compilación e inclusión.
# Descripción: Se ha actualizado el CMakeLists para que reconozca el nuevo directorio 'telemetry', asegurando que los ficheros 'telemetry_manager.c' y 'telemetry_task.c' se compilen y sus cabeceras sean encontradas. Esto completa la refactorización de la lógica de telemetría.

file(GLOB component_sources
    "*.c"
    "core/*.c"
    "screens/*.c"
    "actions/*.c"
    "assets/styles/*.c"
    "buttons/*.c"
    "telemetry/*.c"
)

# Buscar y añadir automáticamente TODOS los ficheros .c del directorio de imágenes.
file(GLOB asset_sources "assets/images/*.c")
list(APPEND component_sources ${asset_sources})

idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS
        "."
        "buttons"
        "actions"
        "core"
        "screens"
        "assets/styles"
        "assets/fonts"
        "assets/images"
        "telemetry"
    PRIV_INCLUDE_DIRS "."
    REQUIRES
        lvgl
        bsp
        core
        screen_manager
        wifi_portal
        web_server
)
