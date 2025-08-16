# Fecha: 16/08/2025 - 09:41 
# Fichero: components/service_screen/CMakeLists.txt
# Último cambio: Actualizada la dependencia de 'diymon_bsp' a 'bsp' para resolver el error de compilación.
# Descripción: Fichero de compilación para la pantalla de servicio. Se ha corregido el nombre de la dependencia del BSP para que coincida con el nuevo nombre del directorio ('bsp'), solucionando el error 'Failed to resolve component'.

idf_component_register(SRCS "service_screen.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                        log
                        bsp
)
