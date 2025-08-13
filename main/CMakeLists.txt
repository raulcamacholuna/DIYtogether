# Fichero: ./main/CMakeLists.txt
# Fecha: 13/08/2025 - 05:44 
# Último cambio: Eliminada la dependencia de iot_button.
# Descripción: Se ha eliminado la dependencia del componente iot_button que ya no es necesario después de limpiar el código de ejemplo de main.c.

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
