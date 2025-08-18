/* Fecha: 18/08/2025 - 06:53  */
/* Fichero: components/ui/actions.h */
/* Último cambio: Eliminada la acción ACTION_ID_ENABLE_FILE_SERVER para unificar la configuración en un único modo. */
/* Descripción: Define la interfaz para el sistema de acciones, incluyendo los IDs de acción y la función principal de ejecución. Es el punto central para entender qué interacciones de usuario son posibles. */

#ifndef ACTIONS_H
#define ACTIONS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- El Modelo de Datos para las Acciones ---
typedef enum {
    // Acciones del panel de jugador (superior 1)
    ACTION_ID_COMER,          // 0
    ACTION_ID_EJERCICIO,      // 1
    ACTION_ID_ATACAR,         // 2

    // Acciones del panel de administración (superior 2)
    ACTION_ID_BRIGHTNESS_CYCLE,       // 3
    ACTION_ID_TOGGLE_SCREEN,          // 4
    ACTION_ID_ADMIN_PLACEHOLDER,      // 5

    // Acciones del panel de evolución (lateral)
    ACTION_ID_EVO_FIRE,       // 6
    ACTION_ID_EVO_WATER,      // 7
    ACTION_ID_EVO_EARTH,      // 8
    ACTION_ID_EVO_WIND,       // 9
    ACTION_ID_EVO_BACK,       // 10
    
    // Acciones del panel de configuración (superior 3)
    ACTION_ID_ACTIVATE_CONFIG_MODE,   // 11
    ACTION_ID_RESET_ALL,              // 12
    ACTION_ID_CONFIG_PLACEHOLDER,     // 13

    ACTION_ID_COUNT 
} diymon_action_id_t;


// --- El Orquestador de Acciones ---
void execute_diymon_action(diymon_action_id_t action_id);

#ifdef __cplusplus
}
#endif

#endif // ACTIONS_H
