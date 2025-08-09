/*
 * Fichero: ./components/diymon_ui/ui_action_animations.h
 * Fecha: 10/08/2025 - 22:00
 * Último cambio: Añadida la declaración de ui_action_animations_get_player.
 * Descripción: Interfaz pública para el módulo de animaciones. Se añade la
 *              declaración de la función que permite a otros módulos obtener
 *              el reproductor de animación compartido, resolviendo el error de
 *              compilación por declaración implícita.
 */
#ifndef UI_ACTION_ANIMATIONS_H
#define UI_ACTION_ANIMATIONS_H

#include <lvgl.h>
#include "actions.h"
#include "animation_loader.h" // Necesario para que se reconozca el tipo 'animation_t'

/**
 * @brief Crea los objetos y pre-reserva los recursos para las animaciones de acción.
 * @param parent El objeto padre sobre el que se crearán las imágenes.
 */
void ui_action_animations_create(lv_obj_t *parent);

/**
 * @brief Reproduce una animación de acción.
 * @param action_id El ID de la acción a reproducir.
 * @param idle_obj El objeto de la animación de idle, para pausarlo y reanudarlo.
 */
void ui_action_animations_play(diymon_action_id_t action_id, lv_obj_t* idle_obj);

/**
 * @brief Libera los recursos reservados por el módulo de animaciones.
 *        Debe llamarse cuando la pantalla se destruye para evitar fugas de memoria.
 */
void ui_action_animations_destroy(void);

/**
 * @brief Obtiene un puntero al reproductor de animación compartido.
 * @return Puntero a la estructura de animación compartida.
 */
animation_t* ui_action_animations_get_player(void);


#endif // UI_ACTION_ANIMATIONS_H