# Fichero: components/bsp/CMakeLists.txt
# Último cambio: Añadida la dependencia 'esp_lcd_jd9853' para que el sistema de build reconozca el driver de display correcto.
# Descripción: Diagnóstico: Para que el fichero 'bsp_display.c' pueda incluir la cabecera 'esp_lcd_jd9853.h' y utilizar el driver correcto para la placa de 1.47", este CMakeLists.txt debe declarar explícitamente esa dependencia. Solución: Se añade 'esp_lcd_jd9853' a la lista 'REQUIRES'. Esto propaga las rutas de inclusión necesarias, permitiendo que el código C se compile correctamente contra el nuevo driver, que es el paso previo a la selección condicional en 'bsp_display.c'.
# Último cambio: 20/08/2025 - 06:45
idf_component_register(
    SRCS
        "bsp.c"
        "bsp_battery.c"
        "bsp_display.c"
        "bsp_i2c.c"
        "bsp_qmi8658.c"
        "bsp_sdcard.c"
        "bsp_spi.c"
        "bsp_touch.c"
        "bsp_wifi.c"

    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "."

    REQUIRES
        driver
        esp_lcd
        esp_lcd_touch
        sdmmc
        fatfs
        esp_adc
        esp_wifi
        nvs_flash
        lvgl
        esp_lvgl_port
        esp_lcd_gc9a01
        esp_lcd_touch_cst816s
        esp_lcd_touch_axs5106
        esp_lcd_jd9853 # [CORRECCIÓN] Dependencia añadida para el nuevo driver de display.
)
