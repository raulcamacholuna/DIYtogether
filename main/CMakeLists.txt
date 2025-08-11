# CMakeLists.txt para el componente principal de la aplicación

idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
    INCLUDE_DIRS "."
    
    REQUIRES
        # Tus componentes de alto nivel
        diymon_bsp
        diymon_core
        diymon_ui
        wifi_portal
        screen_manager # <-- ANOTACIÓN: Añadimos el nuevo componente.

        # Dependencias de bajo nivel que usan tus componentes
        lvgl
        esp_lcd
        esp_lcd_touch
        driver
)