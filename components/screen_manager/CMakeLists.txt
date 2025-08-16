# Fecha: 16/08/2025 - 09:27 
# Fichero: components/screen_manager/CMakeLists.txt
# Último cambio: Actualizada la dependencia de 'diymon_bsp' a 'bsp' para resolver el error de compilación.
# Descripción: Fichero de compilación para el gestor de pantalla. Se ha corregido el nombre de la dependencia del BSP para que coincida con el nuevo nombre del directorio, solucionando el error 'Failed to resolve component'.

idf_component_register(SRCS "screen_manager.c"
                    INCLUDE_DIRS "."
                    REQUIRES 
                        log
                        bsp # Corregido de 'diymon_bsp' a 'bsp'
                        )
