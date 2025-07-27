# CMakeLists.txt para el componente 'main' del proyecto DIYMON.

idf_component_register(
    SRCS 
        "main.c"
        "hardware_manager.c"

    INCLUDE_DIRS
        "."

    # REQUIRES: Lista de otros componentes del proyecto de los que 'main' depende.
    # En ESP-IDF v5.x, componentes base como 'esp_log' y 'freertos' se incluyen automáticamente.
    REQUIRES
        # --- Componentes del sistema ESP-IDF ---
        nvs_flash       # Para guardar datos de forma persistente.

        # --- Componentes de la Interfaz Gráfica ---
        esp_lvgl_port   # El componente que adapta LVGL para el ESP32.

        # --- Componentes de Hardware Específicos (BSP) ---
        # Este componente debe existir en tu carpeta 'components' y contener
        # todos los ficheros bsp_*.c y bsp_*.h
        esp_bsp

        # --- Componentes propios de DIYMON ---
        # Nuestro componente 'main' necesita usar la UI, por lo que debe requerir 'diymon_ui'.
        diymon_ui
        
        # --- [NUEVO] El cerebro del juego ---
        # Se añade el nuevo componente que contiene toda la lógica de evolución y combate.
        diymon_core
)