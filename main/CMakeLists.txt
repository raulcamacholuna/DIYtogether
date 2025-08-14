# Fichero: Z:\DIYTOGETHER\DIYtogether\main\CMakeLists.txt
# Fecha: $timestamp
# Último cambio: Eliminada la dependencia del componente obsoleto `service_screen`.
# Descripción: CMakeLists para el componente principal. Se elimina la dependencia de `service_screen` ya que su funcionalidad ha sido reemplazada por la nueva pantalla de configuración `ui_config_screen` dentro del componente `diymon_ui`.

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

        lvgl
        esp_lvgl_port
        esp_lcd
        esp_lcd_touch
        driver
)
