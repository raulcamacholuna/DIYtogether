/*
 * Fichero: ./components/diymon_ui/ui_action_animations.h
 * Fecha: 13/08/2025 - 07:34 
 * Último cambio: Reintroducida la función para obtener el reproductor de animación compartido.
 * Descripción: Interfaz pública para el módulo de animaciones de acción. Se vuelve a exponer la función 'ui_action_animations_get_player' para permitir que otros módulos (como el de la animación de reposo) accedan al búfer de animación compartido.
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
void ui_action_animations_create(lv_obj_t *parent);
void ui_action_animations_play(diymon_action_id_t action_id);
void ui_action_animations_destroy(void);
animation_t* ui_action_animations_get_player(void);

#ifdef __cplusplus
}
#endif

#endif // UI_ACTION_ANIMATIONS_H
