/*
 * Fichero: ./components/diymon_ui/actions.h
 * Fecha: 10/08/2025 - 03:40
 * Último cambio: Modificada la firma de `execute_diymon_action`.
 * Descripción: Se cambia la firma de la función para que acepte los datos necesarios directamente como parámetros, en lugar de depender de un objeto de evento.
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- El Modelo de Datos para las Acciones ---
typedef enum {
    ACTION_ID_COMER,
    ACTION_ID_EJERCICIO,
    ACTION_ID_ATACAR,
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