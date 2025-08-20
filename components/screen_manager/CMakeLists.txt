# Fecha: 19/08/2025 - 18:40
# Fichero: components/screen_manager/CMakeLists.txt
# Último cambio: Añadida la dependencia explícita 'esp_lcd' para resolver el error 'esp_lcd_panel_ops.h: No such file or directory'.
# Descripción: Fichero de compilación para el gestor de pantalla. El error de compilación ocurre porque este componente usa 'bsp_api.h', que a su vez incluye cabeceras del componente 'esp_lcd'. Al añadir 'esp_lcd' a la lista 'REQUIRES' de este fichero, se asegura que el sistema de build propague las rutas de inclusión necesarias, resolviendo la dependencia transitiva y el error.

idf_component_register(SRCS "screen_manager.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        log
                        bsp
                        esp_lcd
                        esp_lcd_touch
                        esp_wifi
                        lvgl
                        esp_lvgl_port
                        )
