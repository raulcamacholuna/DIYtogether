/*
 * Fichero: components/diymon_ui/actions.h
 * Descripción: Cabecera para el sistema de acciones dinámicas de DIYMON.
 *              == ESTE FICHERO ES LA ÚNICA FUENTE DE LA VERDAD PARA LOS IDs DE ACCIÓN ==
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- El Modelo de Datos para las Acciones ---
// Este enum define un identificador único para cada posible acción del usuario.
typedef enum {
    ACTION_ID_COMER,
    ACTION_ID_EJERCICIO,
    ACTION_ID_ATACAR,
    
    // ESTA LÍNEA ES CRUCIAL. Es un truco estándar en C para obtener automáticamente
    // el número total de acciones. Siempre debe ser el último elemento.
    ACTION_ID_COUNT 
} diymon_action_id_t;


// --- El Orquestador de Acciones ---
/**
 * @brief Orquestador principal de acciones, llamado por los eventos de los botones.
 * @param e Puntero al evento de LVGL que contiene los datos de la acción.
 */
void execute_diymon_action(lv_event_t *e);


#ifdef __cplusplus
}
#endif

#endif // ACTIONS_H