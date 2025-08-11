idf_component_register(SRCS "screen_manager.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        log
                        diymon_bsp # <-- ANOTACIÓN: Dependencia clave del BSP
                        )