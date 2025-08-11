/*
  Fichero: ./components/diymon_ui/actions.h
  Fecha: 11/08/2025 - 22:30
  Último cambio: Corregido el ID de acción para habilitar FTP.
  Descripción: Define los identificadores para todas las acciones de usuario posibles.
               Se reemplaza la acción de borrado de WiFi por la de activación de FTP.
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

    // Acciones del panel de administración
    ACTION_ID_BRIGHTNESS_CYCLE,
    ACTION_ID_TOGGLE_SCREEN,
    ACTION_ID_ENABLE_FTP,       // Reemplaza a ACTION_ID_ERASE_WIFI

    // Acciones del panel de evolución (lateral)
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