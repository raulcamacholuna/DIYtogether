# CMakeLists.txt para el componente diymon_bsp (Versión Final y Completa)

# 1. Definimos la subcarpeta de nuestra placa
set(BOARD_SUBDIR "WS1.9TS")

# 2. Le decimos a CMake que encuentre todos los archivos .c
file(GLOB component_sources "${BOARD_SUBDIR}/*.c")

# 3. Registramos el componente
idf_component_register(
    SRCS ${component_sources}
    
    # [SOLUCIÓN AL ERROR de bsp_api.h]
    # Hacemos que los headers de la carpeta "include" sean públicos
    # y los de la carpeta de la placa sean privados para el componente.
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