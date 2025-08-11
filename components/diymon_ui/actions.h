/*
 * Fichero: ./components/diymon_ui/actions.h
 * Fecha: 11/08/2025 - 14:30
 * Último cambio: Renombrados los IDs de acción para el panel de administración.
 * Descripción: Define los identificadores para todas las acciones de usuario posibles,
 *              incluyendo las del panel de acciones principal y el panel de administración
 *              con sus nuevas funcionalidades.
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- El Modelo de Datos para las Acciones ---
typedef enum {
    // Acciones del panel de jugador (superior)
    ACTION_ID_COMER,
    ACTION_ID_EJERCICIO,
    ACTION_ID_ATACAR,

    // --- ANOTACIÓN: Acciones del panel de administración (nuevas funciones) ---
    ACTION_ID_BRIGHTNESS_CYCLE, // Antes ACTION_ID_LVL_DOWN
    ACTION_ID_TOGGLE_SCREEN,    // Antes ACTION_ID_SCREEN_OFF
    ACTION_ID_ERASE_WIFI,       // Antes ACTION_ID_LVL_UP

    // Acciones del panel de evolución (lateral) - Se mantienen
    ACTION_ID_EVO_FIRE,
    ACTION_ID_EVO_WATER,
    ACTION_ID_EVO_EARTH,
    ACTION_ID_EVO_WIND,
    ACTION_ID_EVO_BACK,
    
    ACTION_ID_COUNT 
} diymon_action_id_t;


// --- El Orquestador de Acciones ---
/**
 * @brief Orquestador principal de acciones, llamado por los eventos de los botones.
 * @param action_id El identificador de la acción a ejecutar.
 * @param idle_obj Un puntero al objeto de animación de idle para poder pausarlo.
 */
void execute_diymon_action(diymon_action_id_t action_id, lv_obj_t* idle_obj);


#ifdef __cplusplus
}
#endif

#endif // ACTIONS_H