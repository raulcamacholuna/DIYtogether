/*
 * Fichero: ./components/diymon_ui/actions.h
 * Fecha: 12/08/2025 - 10:30
 * Último cambio: Añadidos IDs para las acciones del panel de administración.
 * Descripción: Define los identificadores para todas las acciones de usuario posibles, incluyendo las del panel de acciones principal y el nuevo panel de administración.
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- El Modelo de Datos para las Acciones ---
typedef enum {
    // Acciones del panel de jugador
    ACTION_ID_COMER,
    ACTION_ID_EJERCICIO,
    ACTION_ID_ATACAR,

    // Acciones del panel de administración
    ACTION_ID_LVL_DOWN,
    ACTION_ID_SCREEN_OFF,
    ACTION_ID_LVL_UP,
    
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