# Fichero: ./components/diymon_ui/CMakeLists.txt
# Fecha: 09/08/2025 - 09:25
# Último cambio: Corregida la sintaxis de los comentarios a '#'.
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