/* Fichero: components/ui/actions/action_brightness.c */
/* Descripción: Se actualiza la llamada a 'screen_manager_set_brightness' para pasar 'true' al parámetro 'save_to_nvs'. Esto asegura que cuando el usuario cicla el brillo, su selección se guarde explícitamente como la nueva preferencia. */
/* Último cambio: 21/08/2025 - 19:23 */
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
    
    // Llama al gestor de pantalla para aplicar el nuevo nivel de brillo y guardarlo.
    screen_manager_set_brightness(s_brightness_levels[s_current_brightness_idx], true);
}
