/* Fecha: 17/08/2025 - 01:48  */
/* Fichero: components/ui/actions/action_interaction.c */
/* Último cambio: Implementación de las acciones de interacción (comer, ejercicio, atacar). */
/* Descripción: Implementa las funciones que inician las animaciones de acción básicas. Este módulo actúa como un simple despachador, llamando al gestor de animaciones con el ID de acción correcto. */

#include "actions/action_interaction.h"
#include "ui_action_animations.h" // Necesario para reproducir animaciones
#include "actions.h"              // Necesario para los ACTION_ID_*

/**
 * @brief Implementa la acción de 'comer'.
 */
void action_interaction_eat(void) {
    ui_action_animations_play(ACTION_ID_COMER);
}

/**
 * @brief Implementa la acción de 'ejercicio'.
 */
void action_interaction_gym(void) {
    ui_action_animations_play(ACTION_ID_EJERCICIO);
}

/**
 * @brief Implementa la acción de 'atacar'.
 */
void action_interaction_attack(void) {
    ui_action_animations_play(ACTION_ID_ATACAR);
}
