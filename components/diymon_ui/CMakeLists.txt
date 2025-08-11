# Fichero: ./components/diymon_ui/CMakeLists.txt
# Fecha: 11/08/2025 - 08:55
# Último cambio: Añadido welcome_screen.c a la lista de fuentes.
# Descripción: Se ha añadido 'diymon_core' a la lista de requerimientos (REQUIRES)
#              para permitir que la UI pueda consultar el estado de evolución
#              del personaje y cargar las animaciones correspondientes.

# 1. BÚSQUEDA AUTOMÁTICA DE ARCHIVOS FUENTE
file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
)

# --- ANOTACIÓN: Se añade el fichero de la pantalla de bienvenida a la lista. ---
list(APPEND component_sources "welcome_screen.c")


# 2. REGISTRO DEL COMPONENTE
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