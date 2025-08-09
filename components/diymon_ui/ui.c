/*
 * Fichero: ./components/diymon_ui/ui.c
 * Fecha: 10/08/2025 - 03:40
 * Último cambio: Corregida la llamada a `execute_diymon_action`.
 * Descripción: Se elimina la llamada incorrecta a `lv_event_send` y se llama a `execute_diymon_action` con los parámetros correctos, resolviendo los errores de compilación.
 */
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_actions_panel.h" 
#include "esp_log.h"

extern lv_obj_t *g_main_screen_obj; 
extern lv_obj_t *g_idle_animation_obj; 

static const char *TAG = "DIYMON_UI_MAIN";

// --- Callback de evento para los botones que añade logging ---
static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {
        switch(action_id) {
            case ACTION_ID_COMER:     ESP_LOGW(TAG, "BOTÓN PULSADO: Comer");     break;
            case ACTION_ID_EJERCICIO: ESP_LOGW(TAG, "BOTÓN PULSADO: Ejercicio"); break;
            case ACTION_ID_ATACAR:    ESP_LOGW(TAG, "BOTÓN PULSADO: Atacar");    break;
            default: break;
        }
        
        // --- LA SOLUCIÓN ---
        // Se elimina la llamada incorrecta a lv_event_send y se llama directamente
        // a la función de acción con los parámetros correctos.
        execute_diymon_action(action_id, g_idle_animation_obj);
    }
}


// --- Función para conectar los botones a sus acciones ---
static void ui_connect_actions(void) {
    lv_obj_t *eat_btn = ui_actions_panel_get_eat_btn();
    lv_obj_t *gym_btn = ui_actions_panel_get_gym_btn();
    lv_obj_t *atk_btn = ui_actions_panel_get_atk_btn();

    if (eat_btn) lv_obj_add_event_cb(eat_btn, button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_COMER);
    if (gym_btn) lv_obj_add_event_cb(gym_btn, button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EJERCICIO);
    if (atk_btn) lv_obj_add_event_cb(atk_btn, button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ATACAR);
    
    ESP_LOGI(TAG, "Eventos de botones de acción conectados.");
}


void ui_init(void) {
    create_screens();
    
    if (g_main_screen_obj) {
        ui_connect_actions();
        // El callback de borrado ahora no necesita castearse
        lv_obj_add_event_cb(g_main_screen_obj, (lv_event_cb_t)delete_screen_main, LV_EVENT_DELETE, NULL);
    }
    
    lv_screen_load(g_main_screen_obj);
    ESP_LOGI(TAG, "UI modularizada y lista.");
}