/* Fecha: 17/08/2025 - 02:47  */
/* Fichero: components/ui/screens/welcome.h */
/* Último cambio: Refactorizado y movido a 'screens/', renombrado de 'welcome_screen.h'. */
/* Descripción: Define la interfaz pública para la pantalla de bienvenida. Encapsula la lógica de esta vista específica como parte de la refactorización de la UI. */

#ifndef WELCOME_H
#define WELCOME_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Muestra la pantalla de bienvenida.
 *
 * @param on_start_cb Puntero a una función que se llamará cuando se pulse el botón "INICIAR".
 */
void welcome_screen_show(void (*on_start_cb)(void));

#ifdef __cplusplus
}
#endif

#endif // WELCOME_H
