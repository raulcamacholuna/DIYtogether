#ifndef UI_H_
#define UI_H_

#include "lvgl.h"
#include "screens.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_init();
void ui_tick();
void loadScreen(enum ScreensEnum screenId);

// ----- [NUEVO] DECLARACIÓN DE NUESTRAS FUNCIONES PÚBLICAS -----
// Ahora main.c y actions.c sabrán que estas funciones existen.

/**
 * @brief Actualiza el sprite del DIYMON basándose en su estado actual.
 */
void ui_update_diymon_sprite(void);

/**
 * @brief Reproduce una animación de acción (ej: "comer") desde la SD.
 * @param action_name El nombre del fichero GIF (sin la extensión).
 */
void ui_play_action_animation(const char* action_name);


#ifdef __cplusplus
}
#endif

#endif /*UI_H_*/