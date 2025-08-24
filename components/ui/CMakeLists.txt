# Fichero: components/ui/CMakeLists.txt
# Último cambio: 24/08/2025 - 16:02
# Descripción: El sistema de build para el componente UI ya utiliza un patrón ile(GLOB ... "actions/*.c") para encontrar sus ficheros fuente. Por tanto, el nuevo fichero ction_zigbee.c, al ser añadido en el subdirectorio ctions/, será incluido en la compilación automáticamente sin necesidad de modificar este fichero. La dependencia de zigbee_comm ya existe, satisfaciendo los requisitos del nuevo módulo.
file(GLOB component_sources
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
        zigbee_comm
)
