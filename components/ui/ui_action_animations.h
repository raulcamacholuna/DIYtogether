/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_action_animations.h
# Fecha: $timestamp
# Último cambio: Añadida la declaración de la función de pre-reserva de buffer.
# Descripción: Interfaz pública para el módulo de animaciones de acción. Se añade la declaración de `ui_action_animations_preinit_buffer` para que la función sea visible para otros módulos y se resuelva así el error de compilación por declaración implícita.
*/
#ifndef UI_ACTION_ANIMATIONS_H
#define UI_ACTION_ANIMATIONS_H

#include <lvgl.h>
#include "actions.h"
#include "animation_loader.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- OBJETO GLOBAL COMPARTIDO ---
extern lv_obj_t *g_animation_img_obj;

// --- FUNCIONES PÚBLICAS ---
void ui_action_animations_preinit_buffer(void);
void ui_action_animations_create(lv_obj_t *parent);
void ui_action_animations_play(diymon_action_id_t action_id);
void ui_action_animations_destroy(void);
animation_t* ui_action_animations_get_player(void);

#ifdef __cplusplus
}
#endif

#endif // UI_ACTION_ANIMATIONS_H
