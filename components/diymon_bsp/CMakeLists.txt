# Fichero: ./components/diymon_bsp/CMakeLists.txt
# Fecha: 08/08/2025 - 21:50
# Último cambio: Restaurado a la lista de dependencias correcta.
# Descripción: Registro del componente BSP. Se mantiene la lista de dependencias
#              alineada con el hardware real del proyecto.

# 1. Definimos la subcarpeta de nuestra placa
set(BOARD_SUBDIR "WS1.9TS")

# 2. Le decimos a CMake que encuentre todos los archivos .c
file(GLOB component_sources "${BOARD_SUBDIR}/*.c")

# 3. Registramos el componente
idf_component_register(
    SRCS ${component_sources}
    
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "${BOARD_SUBDIR}"
    
    REQUIRES 
        # Drivers estándar
        driver
        esp_lcd
        sdmmc
        fatfs
        esp_adc
        esp_wifi
        esp_driver_i2c
        nvs_flash

        # Drivers gestionados
        lvgl
        esp_lvgl_port
        esp_lcd_touch_cst816s
        esp_lcd_sh8601
)