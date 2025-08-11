#
# Fichero: ./components/ftp_server/CMakeLists.txt
# Fecha: 11/08/2025 - 19:30
# Último cambio: Creación del componente.
# Descripción: Registro y dependencias para el componente del servidor FTP.
#
idf_component_register(SRCS "ftp_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        driver
                        esp_wifi
                        esp_event
                        nvs_flash
                        lvgl
                        esp_lvgl_port
                        log
                        # En una implementación real, aquí se añadiría la librería FTP
                        # como, por ejemplo, 'espressif/esp_ftp_server'
)