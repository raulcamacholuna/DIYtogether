# =====================================================================================
#  CMakeLists.txt para el componente 'diymon_ui'
#  Versión Definitiva: Automatizada, robusta y fácil de mantener.
# =====================================================================================

# 1. BÚSQUEDA AUTOMÁTICA DE ARCHIVOS FUENTE
# -------------------------------------------------------------------------------------
# La función file(GLOB ...) busca todos los archivos que coincidan con los patrones
# y los guarda en la variable 'component_sources'.
# Esto es la clave para que no tengas que editar este archivo nunca más.
#
# - "*.c":           Encuentra todos los .c en la raíz de 'diymon_ui' (ui.c, screens.c,
#                    y tus nuevos archivos de imagen como attack.c, eat.c, etc.).
# - "screens/*.c":   Encuentra los .c que puedas tener en la subcarpeta 'screens'.
# - "actions/*.c":   Encuentra los .c que puedas tener en la subcarpeta 'actions'.
# - "styles/*.c":    Encuentra los .c que puedas tener en la subcarpeta 'styles'.

file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
)

# 2. REGISTRO DEL COMPONENTE
# -------------------------------------------------------------------------------------
# Aquí le decimos a ESP-IDF todo lo que necesita saber sobre nuestro componente.

idf_component_register(
    # SRCS: Usa la lista de archivos que acabamos de encontrar automáticamente.
    SRCS ${component_sources}

    # INCLUDE_DIRS: Permite que otros componentes (como 'main') puedan hacer #include "ui.h".
    INCLUDE_DIRS "."

    # PRIV_INCLUDE_DIRS: Directorios de cabeceras solo para este componente.
    PRIV_INCLUDE_DIRS "."
    
    # REQUIRES: La lista de otros componentes que 'diymon_ui' necesita para funcionar.
    REQUIRES 
        lvgl          # Necesario para todas las funciones de la librería gráfica.
        diymon_bsp    # Necesario para interactuar con el hardware (pantalla, táctil).
        diymon_core   # Necesario para la lógica del juego y la evolución.
)