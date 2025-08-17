# Fecha: 17/08/2025 - 04:32 
# Fichero: components/ui/CMakeLists.txt
# Último cambio: Corregido un error de sintaxis en el comentario de PowerShell y refinada la lógica de compilación de assets.
# Descripción: Se elimina el uso de acentos graves (backticks) en los comentarios para evitar errores del parser de PowerShell. La lógica de CMake ahora es más explícita: se listan los ficheros de assets C requeridos ('BG.c', 'ui_assets.c') y se excluye 'BG_config.c' para resolver definitivamente los errores de enlazador 'multiple definition' y 'undefined reference'.

file(GLOB component_sources
    "*.c"
    "core/*.c"
    "screens/*.c"
    "actions/*.c"
    "assets/styles/*.c"
    "buttons/*.c"
)

# Se añaden explícitamente los ficheros C que definen los assets de imagen.
# Esto evita compilar ficheros redundantes (como BG_config.c, cuyo contenido
# ya está en ui_assets.c) y asegura que todos los símbolos necesarios (como bg_0 de BG.c)
# estén disponibles para el enlazador.
list(APPEND component_sources
    "assets/images/BG.c"
    "assets/images/ui_assets.c"
)

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
