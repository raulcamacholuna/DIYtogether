/* Fecha: 18/08/2025 - 06:54  */
/* Fichero: components/ui/actions.c */
/* Último cambio: Eliminado el case para ACTION_ID_ENABLE_FILE_SERVER para reflejar la unificación de los modos de configuración. */
/* Descripción: Orquestador de acciones refactorizado. Se ha eliminado el punto de entrada para el antiguo modo de servidor de ficheros. Ahora, toda la funcionalidad de configuración se inicia a través de ACTION_ID_ACTIVATE_CONFIG_MODE, que activa una UI con un servidor web integrado. */

#include "actions.h"
#include "esp_log.h"

// --- Inclusión de todos los módulos de acción refactorizados ---
#include "actions/action_brightness.h"
#include "actions/action_config_mode.h"
#include "actions/action_evolution.h"
#include "actions/action_interaction.h"
#include "actions/action_screen.h"
#include "actions/action_system.h"
#include "actions/action_performance_mode.h"

static const char *TAG = "DIYMON_ACTIONS";

void execute_diymon_action(diymon_action_id_t action_id) {
    ESP_LOGI(TAG, "Ejecutando acción ID: %d", action_id);

    if (action_id == ACTION_ID_ACTIVATE_CONFIG_MODE) {
        action_config_mode_start();
        return;
    }
    
    switch(action_id) {
        // --- Acciones de Interacción ---
        case ACTION_ID_COMER:
            action_interaction_eat();
            break;
        case ACTION_ID_EJERCICIO:
            action_interaction_gym();
            break;
        case ACTION_ID_ATACAR:
            action_interaction_attack();
            break;

        // --- Acciones de UI/Sistema ---
        case ACTION_ID_BRIGHTNESS_CYCLE:
            action_brightness_cycle();
            break;
        case ACTION_ID_TOGGLE_SCREEN:
            action_screen_toggle();
            break;

        // --- Acciones de Sistema (Reset) ---
        case ACTION_ID_RESET_ALL:
            action_system_reset_all();
            break;

        // --- Acciones de Evolución ---
        case ACTION_ID_EVO_FIRE:
            action_evolution_branch(1); // 1 = Fuego
            break;
        case ACTION_ID_EVO_WATER:
            action_evolution_branch(2); // 2 = Agua
            break;
        case ACTION_ID_EVO_EARTH:
            action_evolution_branch(3); // 3 = Tierra
            break;
        case ACTION_ID_EVO_WIND:
            action_evolution_branch(4); // 4 = Viento
            break;
        case ACTION_ID_EVO_BACK:
            action_evolution_devolve();
            break;
        
        // --- Acciones Placeholder (no hacen nada) ---
        case ACTION_ID_ADMIN_PLACEHOLDER:
            ESP_LOGI(TAG, "Acción Placeholder (ID %d) ejecutada. No se realiza ninguna operación.", action_id);
            break;

        case ACTION_ID_PERFORMANCE_MODE:
            action_performance_mode_toggle();
            break;

        default:
            ESP_LOGW(TAG, "Acción con ID %d desconocida.", action_id);
            break;
    }
}
