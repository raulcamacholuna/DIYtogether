/*
 * =====================================================================================
 *       Filename:  actions.h
 *    Description:  Cabecera para el sistema de acciones dinámicas y basadas en datos de DIYMON.
 *        Version:  4.2 (Final y Correcto)
 * =====================================================================================
 */
#ifndef EEZ_LVGL_UI_ACTIONS_H
#define EEZ_LVGL_UI_ACTIONS_H

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
void execute_diymon_action(lv_event_t *e);

// --- Funciones de Compatibilidad con EEZ Studio ---
extern void action_comer(lv_event_t * e);
extern void action_ejercicio(lv_event_t * e);
extern void action_atacar(lv_event_t * e);
extern void action_idle(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_ACTIONS_H*/