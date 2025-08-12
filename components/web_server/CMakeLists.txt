# Fichero: ./components/web_server/CMakeLists.txt
# Fecha: 13/08/2025 - 07:20 pm
# Último cambio: Añadido web_server_page.h a las cabeceras privadas.
# Descripción: Registro y dependencias para el componente del servidor web. Se incluye el nuevo fichero de cabecera que contiene el HTML.

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
                        diymon_bsp
                        sdmmc
)
