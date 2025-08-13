/*
 * Fichero: ./components/diymon_ui/ui_action_animations.h
 * Fecha: $timestamp
 * Último cambio: Simplificada la interfaz al eliminar la gestión de buffer compartido.
 * Descripción: Interfaz pública para el módulo de animaciones de acción. Se ha eliminado la función `ui_action_animations_get_player` ya que el sistema ya no utiliza un búfer de animación compartido.
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

#ifdef __cplusplus
}
#endif

#endif // UI_ACTION_ANIMATIONS_H