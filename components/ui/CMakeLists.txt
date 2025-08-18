# Fecha: 18/08/2025 - 06:55 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Eliminada la dependencia 'wifi_portal' al centralizar la configuración.
# Descripción: Se ha eliminado 'wifi_portal' de la lista de dependencias. La funcionalidad de portal cautivo ya no se utiliza, ya que el único modo de configuración es el servidor web accesible desde la UI, que es manejado por los componentes 'bsp' (para el AP) y 'web_server'.

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
        web_server
)
