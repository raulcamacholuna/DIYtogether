# Fichero: main/CMakeLists.txt
# Último cambio: Eliminada la directiva 'target_link_libraries' explícita, que ya no es necesaria.
# Descripción: Con la refactorización de 'platformio.ini' que ahora filtra los ficheros fuente a nivel de entorno, el sistema de build de ESP-IDF gestiona correctamente las dependencias transitivas a través de la directiva 'REQUIRES'. La eliminación de este enlace explícito ('target_link_libraries') simplifica el fichero, lo alinea con las prácticas estándar del framework y elimina una solución que abordaba un síntoma, no la causa raíz del problema de enlazado.
# Último cambio: 20/08/2025 - 04:56
idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"
    INCLUDE_DIRS "."
    
    REQUIRES
        bsp
        core
        ui
        web_server
        screen_manager
        driver
        nvs_flash
        fatfs
        sdmmc
        lvgl
        esp_lvgl_port
        esp_lcd
        esp_lcd_touch
)

# La directiva 'target_link_libraries' ha sido eliminada. La dependencia se resuelve correctamente a través de 'REQUIRES bsp'.
