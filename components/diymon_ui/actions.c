/*
 * Archivo: actions.c
 * Descripción: Manejador de eventos de la UI.
 * Versión: 7.0 (Adaptado a la nueva estructura de acciones)
 */

#include "actions.h"
#include "esp_log.h"
#include "ui.h"

static const char *TAG = "DIYMON_ACTIONS";

void execute_diymon_action(lv_event_t *e) {
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    
    switch(action_id) {
        case ACTION_ID_COMER:
            ESP_LOGI(TAG, "Acción: Comer");
            ui_play_action_animation("eat"); // Llama al GIF "eat.gif"
            break;
        case ACTION_ID_EJERCICIO:
            ESP_LOGI(TAG, "Acción: Entrenar");
            ui_play_action_animation("train"); // Llama al GIF "train.gif"
            break;
        case ACTION_ID_ATACAR:
            ESP_LOGI(TAG, "Acción: Atacar");
            ui_play_action_animation("attack"); // Llama al GIF "attack.gif"
            break;
        default:
            ESP_LOGW(TAG, "Acción ID %d no reconocida.", action_id);
            break;
    }
}