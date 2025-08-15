# Fecha: 15/08/2025 - 04:09 
# Fichero: Z:\DIYTOGETHER\DIYtogether\main\CMakeLists.txt
# Último cambio: Añadidas dependencias explícitas (nvs_flash, fatfs, sdmmc) para mayor robustez.
# Descripción: Se han añadido las dependencias `nvs_flash`, `fatfs` y `sdmmc` a la lista de `REQUIRES`. Aunque estas dependencias podían ser resueltas de forma transitiva a través de otros componentes (como el BSP), declararlas explícitamente aquí asegura que el componente `main` siempre se enlace correctamente con estos sistemas base, de los cuales depende para el arranque (NVS) y el montaje del sistema de ficheros (FAT/SD).

idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
    INCLUDE_DIRS "."
    
    REQUIRES
        # Componentes de la aplicación
        diymon_bsp
        diymon_core
        diymon_ui
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
