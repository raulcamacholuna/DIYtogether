/*
 * Fichero: ./components/diymon_ui/ui_action_animations.h
 * Fecha: 10/08/2025 - 03:15
 * Último cambio: Creación del fichero.
 * Descripción: Interfaz pública para el módulo que gestiona las animaciones de las acciones (comer, atacar, etc.).
 */
#ifndef UI_ACTION_ANIMATIONS_H
#define UI_ACTION_ANIMATIONS_H

#include <lvgl.h>
#include "actions.h" // Para diymon_action_id_t

/**
 * @brief Crea los objetos de imagen para todas las animaciones de acción.
 * @param parent El objeto padre sobre el que se crearán las imágenes.
 */
void ui_action_animations_create(lv_obj_t *parent);

/**
 * @brief Reproduce una animación de acción.
 * @param action_id El ID de la acción a reproducir.
 * @param idle_obj El objeto de la animación de idle, para pausarlo y reanudarlo.
 */
void ui_action_animations_play(diymon_action_id_t action_id, lv_obj_t* idle_obj);

#endif // UI_ACTION_ANIMATIONS_H