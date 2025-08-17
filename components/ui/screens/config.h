/* Fecha: 17/08/2025 - 02:48  */
/* Fichero: components/ui/screens/config.h */
/* Último cambio: Refactorizado y movido a 'screens/', renombrado de 'ui_config_screen.h'. */
/* Descripción: Define la interfaz pública para la pantalla de configuración. Encapsula la lógica de esta vista específica como parte de la refactorización de la UI. */

#ifndef CONFIG_H
#define CONFIG_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea y muestra la pantalla de configuración estática con una imagen de fondo y un botón de reinicio.
 */
void config_screen_show(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H
