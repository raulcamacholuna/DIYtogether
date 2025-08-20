# Fecha: 19/08/2025 - 03:57 
# Fichero: components_dependencies/esp_lcd_gc9a01/CMakeLists.txt
# Último cambio: Creación del fichero de compilación para el controlador GC9a01.
# Descripción: Fichero de compilación CMake que registra el código fuente y las cabeceras del driver GC9a01, declarando su dependencia de 'esp_lcd'. Esto permite al sistema de build compilarlo como un componente válido.

idf_component_register(SRCS "esp_lcd_gc9a01.c"
                      INCLUDE_DIRS "include"
                      REQUIRES esp_lcd)
