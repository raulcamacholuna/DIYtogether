# Fichero: ./components/web_server/CMakeLists.txt
# Fecha: 12/08/2025 - 13:00
# Último cambio: Creación del componente.
# Descripción: Registro y dependencias para el componente del servidor web.

idf_component_register(SRCS "web_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        driver
                        esp_wifi
                        esp_event
                        nvs_flash
                        log
                        esp_http_server
                        diymon_bsp
                        sdmmc
)