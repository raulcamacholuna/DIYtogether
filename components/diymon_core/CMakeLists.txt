idf_component_register(SRCS "diymon_evolution.c"
                    INCLUDE_DIRS "include"
                    # Le damos permiso para usar tanto los logs como la memoria flash
                    REQUIRES "log" "nvs_flash"
                    )