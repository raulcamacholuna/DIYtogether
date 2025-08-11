idf_component_register(SRCS "wifi_portal.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        driver
                        esp_wifi
                        esp_http_server
                        nvs_flash
                        esp_event
                        lvgl
                        esp_lvgl_port
                        )