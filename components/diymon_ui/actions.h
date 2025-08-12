/*
  Fichero: ./components/diymon_ui/actions.h
  Fecha: 14/08/2025 - 10:15 am
  Último cambio: Refactorizada la firma de la función de ejecución de acciones.
  Descripción: Define los identificadores para todas las acciones de usuario. La firma de `execute_diymon_action` se ha simplificado para no requerir el objeto de animación de reposo, ya que ahora se gestiona de forma global.
*/
#ifndef ACTIONS_H
#define ACTIONS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- El Modelo de Datos para las Acciones ---
typedef enum {
    // Acciones del panel de jugador (superior 1)
    ACTION_ID_COMER,
    ACTION_ID_EJERCICIO,
    ACTION_ID_ATACAR,

    // Acciones del panel de administración (superior 2)
    ACTION_ID_BRIGHTNESS_CYCLE,
    ACTION_ID_TOGGLE_SCREEN,
    ACTION_ID_ADMIN_PLACEHOLDER,

    // Acciones del panel de evolución (lateral)
    ACTION_ID_EVO_FIRE,
    ACTION_ID_EVO_WATER,
    ACTION_ID_EVO_EARTH,
    ACTION_ID_EVO_WIND,
    ACTION_ID_EVO_BACK,
    
    // Acciones del panel de configuración (superior 3)
    ACTION_ID_RESET_ALL,
    ACTION_ID_ENABLE_FTP,
    ACTION_ID_CONFIG_PLACEHOLDER,

    ACTION_ID_COUNT 
} diymon_action_id_t;


// --- El Orquestador de Acciones ---
/**
 * @brief Orquestador principal de acciones, llamado por los eventos de los botones.
 * @param action_id El identificador de la acción a ejecutar.
 */
void execute_diymon_action(diymon_action_id_t action_id);


#ifdef __cplusplus
}
#endif

#endif // ACTIONS_H