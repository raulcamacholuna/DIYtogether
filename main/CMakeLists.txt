# Fecha: 18/08/2025 - 06:56 
# Fichero: main/CMakeLists.txt
# Último cambio: Eliminada la dependencia 'wifi_portal' al centralizar la configuración.
# Descripción: Se ha eliminado 'wifi_portal' de la lista de dependencias de 'main'. Esta funcionalidad ha quedado obsoleta tras unificar el modo de configuración en un único servidor web accesible desde la UI, simplificando la estructura de dependencias del proyecto.

idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
    INCLUDE_DIRS "."
    
    REQUIRES
        # Componentes de la aplicación
        bsp
        core
        ui
        web_server
        screen_manager

        # Drivers y librerías de sistema base
        driver
        nvs_flash
        fatfs
        sdmmc
        
        # Componentes de UI y display
        lvgl
        esp_lvgl_port
        esp_lcd
        esp_lcd_touch
)
