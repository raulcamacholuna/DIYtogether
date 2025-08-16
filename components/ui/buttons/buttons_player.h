/* Fecha: 16/08/2025 - 09:06  */
/* Fichero: components/ui/buttons/buttons_player.h */
/* Último cambio: Creación del fichero de cabecera para el sub-componente de botones del jugador. */
/* Descripción: Define la interfaz pública para crear y gestionar los botones de 'Comer', 'Ejercicio' y 'Atacar'. Este módulo encapsula la creación de los objetos LVGL y expone funciones para conectar sus eventos. */

#ifndef BUTTONS_PLAYER_H
#define BUTTONS_PLAYER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea los botones de acción del jugador (Comer, Ejercicio, Atacar).
 * 
 * @param parent El objeto LVGL padre sobre el cual se crearán los botones.
 */
void ui_buttons_player_create(lv_obj_t *parent);

/**
 * @brief Conecta los eventos de clic de los botones del jugador a sus acciones correspondientes.
 */
void ui_buttons_player_connect_events(void);

/**
 * @brief Obtiene el manejador del botón 'Comer'.
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* ui_buttons_player_get_eat_btn(void);

/**
 * @brief Obtiene el manejador del botón 'Ejercicio'.
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* ui_buttons_player_get_gym_btn(void);

/**
 * @brief Obtiene el manejador del botón 'Atacar'.
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* ui_buttons_player_get_atk_btn(void);


#ifdef __cplusplus
}
#endif

#endif // BUTTONS_PLAYER_H
