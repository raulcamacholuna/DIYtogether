idf_component_register(
    SRCS "esp_lcd_touch_axs5106.c"
    INCLUDE_DIRS "include"
    
    # [CORRECCIÓN] Le recordamos quién es su padre y qué otras herramientas necesita
    REQUIRES 
        "esp_lcd"
        "esp_lcd_touch" # ¡El nombre correcto de su padre!
        "driver"
        "log"
)