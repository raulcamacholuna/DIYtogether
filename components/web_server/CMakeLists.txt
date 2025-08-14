# Fichero: Z:\DIYTOGETHER\DIYtogether\components\web_server\CMakeLists.txt
# Fecha: `$timestamp
# Último cambio: Corregido el fichero para que sea sintácticamente válido en CMake.
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
