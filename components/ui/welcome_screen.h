#ifndef WELCOME_SCREEN_H
#define WELCOME_SCREEN_H

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

#endif // WELCOME_SCREEN_H