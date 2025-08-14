# Fichero: Z:\DIYTOGETHER\DIYtogether\main\CMakeLists.txt
# Fecha: `$timestamp
# Último cambio: Corregido el fichero para que sea sintácticamente válido en CMake.
# Descripción: CMakeLists para el componente principal 'main', que enlaza todos los subcomponentes del proyecto.

idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
    INCLUDE_DIRS "."
    
    REQUIRES
        diymon_bsp
        diymon_core
        diymon_ui
        wifi_portal
        web_server
        screen_manager
        service_screen

        lvgl
        esp_lcd
        esp_lcd_touch
        driver
)
