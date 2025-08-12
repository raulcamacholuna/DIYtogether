/*
 * Fichero: ./components/diymon_ui/ui_action_animations.h
 * Fecha: 14/08/2025 - 10:00 am
 * Último cambio: Refactorizado para arquitectura de animación compartida.
 * Descripción: Interfaz pública para el módulo de animaciones de acción. Expone el objeto de imagen global y adapta las funciones para operar sobre un búfer de animación compartido.
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
/**
 * @brief Puntero al objeto de imagen que servirá como "lienzo" para todas las animaciones.
 *        Se define en ui_action_animations.c y se declara aquí para ser accesible
 *        por otros módulos de la UI que necesiten invalidarlo.
 */
extern lv_obj_t *g_animation_img_obj;


// --- FUNCIONES PÚBLICAS ---

/**
 * @brief Crea el objeto de imagen de animación y pre-reserva el búfer de memoria compartido.
 * @param parent El objeto padre sobre el que se creará la imagen.
 */
void ui_action_animations_create(lv_obj_t *parent);

/**
 * @brief Reproduce una animación de acción, tomando control del búfer compartido.
 * @param action_id El ID de la acción a reproducir (COMER, EJERCICIO, ATACAR).
 */
void ui_action_animations_play(diymon_action_id_t action_id);

/**
 * @brief Libera el búfer de memoria compartido y otros recursos.
 *        Debe llamarse cuando la pantalla se destruye para evitar fugas de memoria.
 */
void ui_action_animations_destroy(void);

/**
 * @brief Obtiene un puntero al reproductor de animación que contiene el búfer compartido.
 *        Necesario para que el módulo de animación de reposo (idle) pueda usar el mismo búfer.
 * @return Puntero a la estructura de animación compartida.
 */
animation_t* ui_action_animations_get_player(void);

#ifdef __cplusplus
}
#endif

#endif // UI_ACTION_ANIMATIONS_H