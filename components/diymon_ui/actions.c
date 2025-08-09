/*
 * Fichero: ./components/diymon_ui/actions.c
 * Fecha: 10/08/2025 - 03:15
 * Último cambio: Delegada la lógica de visualización al módulo `ui_action_animations`.
 * Descripción: Este fichero ahora actúa como un despachador puro. Su única responsabilidad es recibir un evento de acción y llamar al módulo correspondiente para que reproduzca la animación visual.
 */
#include "actions.h"
#include "ui_action_animations.h" // Incluimos el nuevo módulo de animaciones
#include "esp_log.h"

static const char *TAG = "DIYMON_ACTIONS";

// Se necesita una referencia al objeto de idle para pasárselo al reproductor de animaciones.
extern lv_obj_t *g_idle_animation_obj; // Declaración extern, se definirá en ui.c

void execute_diymon_action(lv_event_t *e) {
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    lv_obj_t *idle_obj = (lv_obj_t*)lv_event_get_param(e); // El objeto idle se pasa como parámetro del evento

    ESP_LOGI(TAG, "Evento de acción recibido para ID: %d. Delegando a reproductor.", action_id);
    
    // Simplemente llamamos al nuevo módulo para que se encargue de la parte visual
    ui_action_animations_play(action_id, idle_obj);
}