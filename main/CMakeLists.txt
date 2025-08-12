# Fichero: ./main/CMakeLists.txt
# Fecha: 12/08/2025 - 13:00
# Último cambio: Actualizadas las dependencias para usar el nuevo servidor web.
# Descripción: Se elimina la dependencia del componente `ftp_server` y se añade
#              la dependencia del nuevo componente `web_server`.

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