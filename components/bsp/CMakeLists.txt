# Fecha: 16/08/2025 - 09:32 
# Fichero: components/bsp/CMakeLists.txt
# Último cambio: Renombrado el componente de 'diymon_bsp' a 'bsp' y corregido un error de copiado.
# Descripción: Fichero de compilación del Board Support Package (BSP). Se ha corregido la dependencia de 'diymon_ui' a 'ui' para que coincida con el nuevo nombre del componente y se han eliminado referencias a componentes que ya no existen, resolviendo así el error de compilación "Failed to resolve component".

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
        esp_driver_i2c
        nvs_flash
        lvgl
        esp_lvgl_port
        esp_lcd_touch_cst816s
        esp_lcd_sh8601
)
