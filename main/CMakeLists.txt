idf_component_register(SRCS "hardware_manager.c" "main.c"
                       # Añade aquí otros ficheros .c o .cpp que tengas en el directorio 'main'

                       # Esto añade el directorio 'main' a la ruta de inclusión para este componente.
                       INCLUDE_DIRS "."

                       # Lista de otros componentes de los que 'main' depende.
                       REQUIRES
                           # --- Componentes del sistema ESP-IDF ---
                           nvs_flash

                           # --- Componentes de la Interfaz Gráfica ---
                           esp_lvgl_port
                           lvgl

                           # --- Componentes de Hardware y propios ---
                           diymon_bsp
                           diymon_ui
                           diymon_core
)