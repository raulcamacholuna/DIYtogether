/*
 * Archivo: actions.h
 * Descripción: Declaraciones privadas para el módulo de acciones.
 * Versión: 8.0
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include "ui.h" // [AÑADIDO] Incluimos la API pública para obtener diymon_action_id_t.

// [ELIMINADO] El enum diymon_action_id_t se ha movido a ui.h.

// --- DECLARACIONES DE FUNCIONES ---
// Esta es la función de callback que se define en actions.c.
void execute_diymon_action(lv_event_t *e);

#endif // ACTIONS_H