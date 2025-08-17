/* Fecha: 17/08/2025 - 03:32  */
/* Fichero: components/ui/screens/config.h */
/* Último cambio: Renombrada la función 'config_screen_show' a 'config_show' para estandarizar y eliminar redundancia. */
/* Descripción: Define la interfaz pública para la pantalla de configuración. La estandarización de nombres (eliminando '_screen') mejora la legibilidad y sigue el principio DRY, ya que el contexto del directorio 'screens' es suficiente. */

#ifndef CONFIG_H
#define CONFIG_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea y muestra la pantalla de configuración estática con una imagen de fondo y un botón de reinicio.
 */
void config_show(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H
