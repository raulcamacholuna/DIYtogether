/* Fecha: 17/08/2025 - 01:51  */
/* Fichero: components/ui/actions.c */
/* Último cambio: Refactorizado para delegar la ejecución de acciones a módulos específicos. */
/* Descripción: Orquestador de acciones refactorizado. Este fichero ya no contiene la lógica de implementación de las acciones. En su lugar, incluye las cabeceras de los módulos de acción (ction_*.h) y, en la función xecute_diymon_action, simplemente llama a la función correspondiente de cada módulo. Esto completa la refactorización, haciendo que este fichero sea un director de orquesta limpio y fácil de mantener. */

#include "actions.h"
#include "esp_log.h"

// --- Inclusión de todos los módulos de acción refactorizados ---
#include "actions/action_brightness.h"
#include "actions/action_config_mode.h"
#include "actions/action_evolution.h"
#include "actions/action_interaction.h"
#include "actions/action_screen.h"
#include "actions/action_system.h"

static const char *TAG = "DIYMON_ACTIONS";

void execute_diymon_action(diymon_action_id_t action_id) {
    ESP_LOGI(TAG, "Ejecutando acción ID: %d", action_id);

    // El modo de configuración es especial porque bloquea otras acciones.
    // La lógica de bloqueo ahora está dentro del propio módulo.
    if (action_id == ACTION_ID_ACTIVATE_CONFIG_MODE) {
        action_config_mode_start();
        return;
    }
    
    // Aquí podríamos añadir una comprobación para no ejecutar acciones si
    // el modo config está activo, pero esa lógica ahora reside en cada módulo si es necesario.
    // Por simplicidad, asumimos que la UI no permite disparar otras acciones en ese modo.

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

        // --- Acciones de Sistema (Modo Operación y Reset) ---
        case ACTION_ID_ENABLE_FILE_SERVER:
            action_system_enable_file_server();
            break;
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
        case ACTION_ID_CONFIG_PLACEHOLDER:
            ESP_LOGI(TAG, "Acción Placeholder (ID %d) ejecutada. No se realiza ninguna operación.", action_id);
            break;

        default:
            ESP_LOGW(TAG, "Acción con ID %d desconocida.", action_id);
            break;
    }
}
