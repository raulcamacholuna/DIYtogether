# Fecha: 17/08/2025 - 03:46 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Excluida la compilación de 'BG_config.c' para resolver un error de 'multiple definition'.
# Descripción: El enlazador reportó que la variable 'bg_config' estaba definida tanto en 'BG_config.c' como en 'ui_assets.c'. Asumiendo que 'ui_assets.c' es el fichero de assets consolidado que ya contiene esta definición, se ha modificado este CMakeLists para excluir explícitamente a 'BG_config.c' de la compilación, solucionando el conflicto de símbolos duplicados.

file(GLOB component_sources
    "*.c"
    "core/*.c"
    "screens/*.c"
    "actions/*.c"
    "assets/styles/*.c"
    "buttons/*.c"
)

# Glob para los ficheros de assets de imagen, y luego se excluye el fichero redundante.
file(GLOB image_assets "assets/images/*.c")
list(REMOVE_ITEM image_assets "${CMAKE_CURRENT_LIST_DIR}/assets/images/BG_config.c")
list(APPEND component_sources ${image_assets})

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
    PRIV_INCLUDE_DIRS "."
    REQUIRES
        lvgl
        bsp
        core
        screen_manager
        wifi_portal
        web_server
)
