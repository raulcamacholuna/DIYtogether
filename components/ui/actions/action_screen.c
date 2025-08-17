/* Fecha: 17/08/2025 - 01:48  */
/* Fichero: components/ui/actions/action_screen.c */
/* Último cambio: Implementación de la lógica de encendido/apagado de pantalla. */
/* Descripción: Módulo que implementa la acción de alternar el estado de la pantalla. Depende del 'screen_manager' para el control físico del display y del 'ui_actions_panel' para ocultar los botones antes de apagar. */

#include "actions/action_screen.h"
#include "screen_manager.h"
#include "ui_actions_panel.h" // Necesario para ocultar los paneles

/**
 * @brief Implementación de la acción de alternar el estado de la pantalla.
 */
void action_screen_toggle(void) {
    if (screen_manager_is_off()) {
        // Si la pantalla está apagada, simplemente la enciende.
        screen_manager_turn_on();
    } else {
        // Si la pantalla está encendida, primero oculta cualquier panel de acción visible
        // y luego apaga la pantalla para una transición limpia.
        ui_actions_panel_hide_all(); 
        screen_manager_turn_off();
    }
}
