# Fecha: 16/08/2025 - 09:38 
# Fichero: main/CMakeLists.txt
# Último cambio: Corregida la dependencia 'diymon_bsp' a 'bsp' para resolver el error de compilación.
# Descripción: El sistema de compilación no encuentra el componente 'diymon_bsp' porque el directorio fue renombrado a 'bsp'. Esta corrección actualiza el nombre en la lista de dependencias de 'main', que es la causa raíz del error "Failed to resolve component 'diymon_bsp' required by component 'main'".

idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
    INCLUDE_DIRS "."
    
    REQUIRES
        # Componentes de la aplicación (con nombres actualizados)
        bsp
        core
        ui
        wifi_portal
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
