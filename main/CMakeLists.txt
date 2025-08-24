# Fichero: main/CMakeLists.txt
# Último cambio: Añadida la dependencia 'zigbee_comm' para permitir que el 'main' utilice la nueva API de comunicación Zigbee.
# Descripción: Diagnóstico: La aplicación principal (main.c) necesita inicializar y controlar el nuevo componente Zigbee. Sin una dependencia explícita, el enlazador no encontrará las funciones de 'zigbee_comm'. Solución: Se añade 'zigbee_comm' a la lista 'REQUIRES'. Esto asegura que el sistema de build enlace correctamente el componente, permitiendo que 'main.c' invoque a 'zigbee_comm_init()' y otras funciones de la API.
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
        zigbee_comm
)
