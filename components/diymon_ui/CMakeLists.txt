# CMakeLists.txt para el componente 'diymon_ui'.
# Versión corregida y optimizada.

# 1. Buscar automáticamente todos los archivos fuente (.c).
#    Esto es más robusto que una lista manual. Si añades un nuevo archivo,
#    lo encontrará solo.
file(GLOB component_sources 
    "*.c"
    "actions/*.c"   # Asumiendo que pudieras tener archivos .c aquí
    "screens/*.c"
    "styles/*.c"
)

# 2. Registrar el componente con toda su información.
idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS "."
    PRIV_INCLUDE_DIRS "."
    
    # --- [LA CORRECCIÓN CLAVE ESTÁ AQUÍ] ---
    REQUIRES 
        # CORRECCIÓN 1: El nombre estándar del componente es "lvgl"
        lvgl

        # CORRECCIÓN 2: AÑADIMOS LA DEPENDENCIA QUE FALTABA
        # La UI necesita "hablar" con el BSP para poder usar la pantalla.
        diymon_bsp

        # La dependencia a diymon_core (si la necesitas)
        diymon_core
)