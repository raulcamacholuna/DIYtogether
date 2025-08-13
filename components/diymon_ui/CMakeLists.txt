# Fichero: ./components/diymon_ui/CMakeLists.txt
# Fecha: 13/08/2025 - 12:43 
# Último cambio: Compilando los assets .c y eliminando referencias a ficheros obsoletos.
# Descripción: Se añaden los ficheros de la carpeta 'assets' al GLOB para que se compilen como código fuente. Se elimina el fichero 'images.c' que ya no es necesario.

file(GLOB component_sources
    "*.c"
    "screens/*.c"
    "actions/*.c"
    "styles/*.c"
    "ui_telemetry.c"
    "assets/*.c" # <-- AÑADIDO: Compilar los nuevos assets en formato .c
)

# La sección COMPONENT_EMBED_FILES y la referencia a images.c se eliminan.

idf_component_register(
    SRCS ${component_sources}
    INCLUDE_DIRS "."
    PRIV_INCLUDE_DIRS "." "assets"
    REQUIRES 
        lvgl
        diymon_bsp
        diymon_core
        screen_manager
        wifi_portal
)
