# CMakeLists.txt para el componente principal de la aplicación

idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
        # ...añade aquí otros ficheros .c que tengas en 'main'...
    INCLUDE_DIRS "."
    
    # [LA SOLUCIÓN FINAL]
    # Declaramos TODAS las dependencias que nuestra aplicación y sus
    # componentes (como el BSP) necesitan para comunicarse entre sí.
    REQUIRES
        # Tus componentes de alto nivel
        diymon_bsp
        diymon_core
        diymon_ui

        # Los drivers de bajo nivel que el BSP expone en su API
        esp_lcd
        esp_lcd_touch
        driver
        esp_driver_i2c
        esp_driver_spi
        sdmmc
        fatfs
)