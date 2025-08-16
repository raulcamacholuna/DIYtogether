# Fecha: 16/08/2025 - 09:28 
# Fichero: components/wifi_portal/CMakeLists.txt
# Último cambio: Actualizada la dependencia de 'diymon_bsp' a 'bsp' para resolver el error de compilación.
# Descripción: CMakeLists del portal WiFi. Se ha corregido el nombre de la dependencia del BSP para que coincida con el nuevo nombre del directorio ('bsp'), solucionando el error 'Failed to resolve component'.

idf_component_register(SRCS "wifi_portal.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        driver
                        esp_wifi
                        esp_http_server
                        nvs_flash
                        esp_event
                        bsp
                        )
