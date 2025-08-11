# Fichero: ./main/CMakeLists.txt
# Fecha: 12/08/2025 - 07:15
# Último cambio: Añadida la dependencia del nuevo componente service_screen.
# Descripción: CMakeLists para el componente principal.

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
        ftp_server
        screen_manager
        service_screen

        lvgl
        esp_lcd
        esp_lcd_touch
        driver
)