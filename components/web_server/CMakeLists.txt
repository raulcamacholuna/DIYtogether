# Fecha: 18/08/2025 - 07:45 
# Fichero: components/web_server/CMakeLists.txt
# Último cambio: Corregido el estilo de comentarios de C-style (/*) a CMake-style (#) para resolver el error de parser.
# Descripción: Registro del componente web_server. El error de compilación se debía a que se estaban usando comentarios de C en un fichero de CMake, lo cual es sintácticamente incorrecto. Se corrige el formato de los comentarios para que el sistema de build pueda procesar el fichero.

idf_component_register(SRCS
                        "web_server.c"
                        "web_server_handlers.c"
                        "web_server_helpers.c"
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
