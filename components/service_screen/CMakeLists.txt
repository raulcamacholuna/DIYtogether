# Fichero: ./components/service_screen/CMakeLists.txt
# Fecha: 12/08/2025 - 09:30
# Último cambio: Eliminadas las dependencias de LVGL.
# Descripción: Registro del componente. Ahora solo depende del BSP y de los logs,
#              ya que el renderizado se hace de forma nativa.

idf_component_register(SRCS "service_screen.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        log
                        diymon_bsp
)