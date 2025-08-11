# Fichero: ./components/wifi_portal/CMakeLists.txt
# Fecha: 12/08/2025 - 09:00
# Último cambio: Añadida la dependencia del componente diymon_bsp.
# Descripción: CMakeLists del portal WiFi. Se añade `diymon_bsp` a la lista
#              de `REQUIRES` para que el compilador pueda encontrar `bsp_api.h`.

idf_component_register(SRCS "wifi_portal.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        driver
                        esp_wifi
                        esp_http_server
                        nvs_flash
                        esp_event
                        diymon_bsp
                        )