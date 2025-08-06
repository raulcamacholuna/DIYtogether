#include "actions.h"
#include "esp_log.h"
#include "ui.h" // [NUEVO] Necesitamos la interfaz pública de la UI

static const char *TAG = "DIYMON_ACTIONS";

void execute_diymon_action(lv_event_t *e) {
    diymon_action_id_t action_id = (diymon_action_id_t)lv_event_get_user_data(e);
    ESP_LOGI(TAG, "Evento dinámico recibido para la acción ID: %d", action_id);

    // [MODIFICADO] Ahora llamamos a la función genérica de la UI para reproducir la animación
    switch(action_id) {
        case ACTION_ID_COMER:
            ui_play_action_animation("comer");
            break;
        case ACTION_ID_EJERCICIO:
            ui_play_action_animation("ejercicio");
            break;
        case ACTION_ID_ATACAR:
            ui_play_action_animation("ataque");
            break;
        default:
            ESP_LOGW(TAG, "Acción ID %d no reconocida.", action_id);
            break;
    }
}

// Las funciones antiguas ya no son necesarias, pero las dejamos por si acaso
void action_comer(lv_event_t *e) { execute_diymon_action(e); }
void action_ejercicio(lv_event_t *e) { execute_diymon_action(e); }
void action_atacar(lv_event_t *e) { execute_diymon_action(e); }
void action_idle(lv_event_t *e) { } // Ya no necesitamos volver a idle desde aquí