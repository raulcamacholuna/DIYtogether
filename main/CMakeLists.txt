idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
        # ...añade aquí otros ficheros .c que tengas en 'main'...
    INCLUDE_DIRS "."
    
    # --- ANOTACIÓN: Declaración de todas las dependencias del componente 'main'. ---
    # Al añadir 'wifi_portal', el sistema de compilación hará que 'wifi_portal.h'
    # sea visible para 'main.c', solucionando el error de "fichero no encontrado".
    REQUIRES
        # Tus componentes de alto nivel
        diymon_bsp
        diymon_core
        diymon_ui
        wifi_portal # <-- ANOTACIÓN: Dependencia añadida.

        # Dependencias de bajo nivel que usan tus componentes
        lvgl
        esp_lcd
        esp_lcd_touch
        driver
        esp_driver_i2c
        esp_driver_spi
        sdmmc
        fatfs
)