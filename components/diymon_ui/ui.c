/*
 * Fichero: ./components/diymon_ui/ui.c
 * Fecha: 10/08/2025 - 04:55
 * Último cambio: Corregido el evento de doble clic inexistente y añadido log para pulsación larga.
 * Descripción: Se elimina el uso del evento `LV_EVENT_DOUBLE_CLICKED` que causaba un error de compilación. En su lugar, se añade el manejo del evento `LV_EVENT_LONG_PRESSED` para registrar las pulsaciones largas en la terminal con un color distintivo.
 */
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_actions_panel.h" 
#include "esp_log.h"

extern lv_obj_t *g_main_screen_obj; 
extern lv_obj_t *g_idle_animation_obj; 

static const char *TAG = "DIYMON_UI_MAIN";

static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    const char* action_name = "Desconocida";

    switch(action_id) {
        case ACTION_ID_COMER:     action_name = "Comer"; break;
        case ACTION_ID_EJERCICIO: action_name = "Ejercicio"; break;
        case ACTION_ID_ATACAR:    action_name = "Atacar"; break;
        default: break;
    }

    if (code == LV_EVENT_CLICKED) {
        ESP_LOGW(TAG, "BOTÓN PULSADO (Click): %s", action_name);
        execute_diymon_action(action_id, g_idle_animation_obj);
    } else if (code == LV_EVENT_LONG_PRESSED) {
        ESP_LOGW(TAG, "BOTÓN PULSADO (Pulsación Larga): %s", action_name);
        // Aquí podrías definir una acción diferente para la pulsación larga si quisieras.
        // Por ahora, ejecuta la misma acción que un click simple.
        execute_diymon_action(action_id, g_idle_animation_obj);
    }
}

static void ui_connect_actions(void) {
    lv_obj_t *eat_btn = ui_actions_panel_get_eat_btn();
    lv_obj_t *gym_btn = ui_actions_panel_get_gym_btn();
    lv_obj_t *atk_btn = ui_actions_panel_get_atk_btn();

    if (eat_btn) {
        lv_obj_add_event_cb(eat_btn, button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_COMER);
        lv_obj_add_event_cb(eat_btn, button_event_cb, LV_EVENT_LONG_PRESSED, (void*)ACTION_ID_COMER);
    }
    if (gym_btn) {
        lv_obj_add_event_cb(gym_btn, button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EJERCICIO);
        lv_obj_add_event_cb(gym_btn, button_event_cb, LV_EVENT_LONG_PRESSED, (void*)ACTION_ID_EJERCICIO);
    }
    if (atk_btn) {
        lv_obj_add_event_cb(atk_btn, button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ATACAR);
        lv_obj_add_event_cb(atk_btn, button_event_cb, LV_EVENT_LONG_PRESSED, (void*)ACTION_ID_ATACAR);
    }
    
    ESP_LOGI(TAG, "Eventos de botones de acción conectados.");
}

void ui_init(void) {
    create_screens();
    
    if (g_main_screen_obj) {
        ui_connect_actions();
        lv_obj_add_event_cb(g_main_screen_obj, (lv_event_cb_t)delete_screen_main, LV_EVENT_DELETE, NULL);
    }
    
    lv_screen_load(g_main_screen_obj);
    ESP_LOGI(TAG, "UI modularizada y lista.");
}