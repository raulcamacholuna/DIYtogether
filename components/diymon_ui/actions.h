/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\actions.h
# Fecha: `$timestamp
# Último cambio: Corregido el fichero para que sea sintácticamente válido en C.
# Descripción: Se actualiza el enumerado de acciones para reflejar el cambio de "modo de configuración" a "servidor de archivos".
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
    ACTION_ID_ENABLE_FILE_SERVER, // Renombrado de ACTION_ID_ENABLE_CONFIG_MODE
    ACTION_ID_CONFIG_PLACEHOLDER,

    ACTION_ID_COUNT 
} diymon_action_id_t;


// --- El Orquestador de Acciones ---
void execute_diymon_action(diymon_action_id_t action_id);


#ifdef __cplusplus
}
#endif

#endif // ACTIONS_H
