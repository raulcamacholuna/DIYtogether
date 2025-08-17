# Fecha: 17/08/2025 - 02:55 
# Fichero: components/bsp/CMakeLists.txt
# Último cambio: Eliminada la dependencia explícita de 'esp_driver_i2c' para resolver un conflicto de compilación.
# Descripción: Se ha eliminado la dependencia 'esp_driver_i2c' de la lista REQUIRES. Requerir sub-componentes de 'driver' individualmente es una práctica no estándar que puede corromper el árbol de dependencias del build system. El componente 'driver' ya se incluye y proporciona acceso a todas las APIs de controladores necesarias (I2C, SPI, etc.), solucionando el fallo de compilación en el componente 'esp_twai'.

set(BOARD_SUBDIR "WS1.9TS")

file(GLOB component_sources "${BOARD_SUBDIR}/*.c")

idf_component_register(
    SRCS ${component_sources}
    
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "${BOARD_SUBDIR}"
    
    REQUIRES 
        driver
        esp_lcd
        sdmmc
        fatfs
        esp_adc
        esp_wifi
        nvs_flash
        lvgl
        esp_lvgl_port
        esp_lcd_touch_cst816s
        esp_lcd_sh8601
)
