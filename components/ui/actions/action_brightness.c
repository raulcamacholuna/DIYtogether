/* Fecha: 17/08/2025 - 01:47  */
/* Fichero: components/ui/actions/action_brightness.c */
/* Último cambio: Implementación de la lógica de brillo, extraída del antiguo 'actions.c'. */
/* Descripción: Implementa la acción de ciclar el brillo. Contiene las variables estáticas para los niveles de brillo y el índice actual, manteniendo este estado aislado del resto del sistema. */

#include "actions/action_brightness.h"
#include "screen_manager.h" // Necesario para llamar a screen_manager_set_brightness

// --- Variables estáticas privadas del módulo de brillo ---
static int s_brightness_levels[] = {25, 50, 75, 100};
static int s_current_brightness_idx = 3; // Corresponde a 100%

/**
 * @brief Implementación de la acción de ciclar el brillo.
 */
void action_brightness_cycle(void) {
    // Avanza al siguiente nivel de brillo, volviendo al principio si es necesario (ciclo)
    s_current_brightness_idx = (s_current_brightness_idx + 1) % 4;
    
    // Llama al gestor de pantalla para aplicar el nuevo nivel de brillo
    screen_manager_set_brightness(s_brightness_levels[s_current_brightness_idx]);
}
