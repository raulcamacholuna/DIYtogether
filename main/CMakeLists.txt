# Fichero: Z:\DIYTOGETHER\DIYtogether\main\CMakeLists.txt
# Fecha: $timestamp
# Último cambio: Añadida la dependencia `esp_lvgl_port` que faltaba.
# Descripción: CMakeLists para el componente principal 'main'. Se ha añadido `esp_lvgl_port` a la lista de dependencias para que el compilador pueda encontrar la cabecera que declara `lvgl_port_register_fs_drv`, solucionando el error de compilación.

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
        esp_lvgl_port # <-- DEPENDENCIA AÑADIDA
        esp_lcd
        esp_lcd_touch
        driver
)
