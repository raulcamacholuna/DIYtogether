# Fecha: 16/08/2025 - 09:29 
# Fichero: components/web_server/CMakeLists.txt
# Último cambio: Actualizada la dependencia de 'diymon_bsp' a 'bsp' para resolver el error de compilación.
# Descripción: Registro y dependencias para el componente del servidor web. Se ha corregido el nombre de la dependencia del BSP para que coincida con el nuevo nombre del directorio, solucionando el error 'Failed to resolve component'.

idf_component_register(SRCS "web_server.c"
                    INCLUDE_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    REQUIRES
                        driver
                        esp_wifi
                        esp_event
                        nvs_flash
                        log
                        esp_http_server
                        bsp
                        sdmmc
)
