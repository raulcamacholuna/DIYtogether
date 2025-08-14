/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui.c
# Fecha: $timestamp
# Último cambio: Eliminada la llamada a `ui_assets_init`.
# Descripción: Se elimina la llamada a la inicialización de assets de esta función. La responsabilidad de cargar los assets ahora recae en el orquestador principal (`main.c`), asegurando que solo se carguen cuando se entra en el modo de aplicación principal y no en los modos de servicio.
*/
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_actions_panel.h"
#include "ui_asset_loader.h"
#include "ui_action_animations.h"
#include "esp_log.h"

extern lv_obj_t *g_main_screen_obj; 

static const char *TAG = "DIYMON_UI_MAIN";

static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    
    bool is_player_action = (action_id >= ACTION_ID_COMER && action_id <= ACTION_ID_ATACAR);

    if (is_player_action) {
        if (code == LV_EVENT_PRESSED) {
            ESP_LOGD(TAG, "Evento de botón PRESSED para la acción de jugador ID: %d", action_id);
            execute_diymon_action(action_id);
        }
    } else {
        if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED) {
            ESP_LOGD(TAG, "Evento de botón CLICK/LONG_PRESS para la acción ID: %d", action_id);
            execute_diymon_action(action_id);
        }
    }
}

static void ui_connect_actions(void) {
    lv_obj_add_event_cb(ui_actions_panel_get_eat_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_COMER);
    lv_obj_add_event_cb(ui_actions_panel_get_gym_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EJERCICIO);
    lv_obj_add_event_cb(ui_actions_panel_get_atk_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_ATACAR);
    lv_obj_add_event_cb(ui_actions_panel_get_brightness_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_BRIGHTNESS_CYCLE);
    lv_obj_add_event_cb(ui_actions_panel_get_toggle_screen_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_TOGGLE_SCREEN);
    lv_obj_add_event_cb(ui_actions_panel_get_admin_placeholder_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_ENABLE_FILE_SERVER);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_fire_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_FIRE);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_water_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_WATER);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_earth_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_EARTH);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_wind_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_WIND);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_back_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_BACK);
    ESP_LOGI(TAG, "Eventos de todos los botones de acción conectados.");
}

void ui_preinit(void) {
    ESP_LOGI(TAG, "Pre-inicializando UI: reservando buffer de animación...");
    ui_action_animations_preinit_buffer();
}

void ui_init(void) {
    // La inicialización de assets (ui_assets_init) ya no se hace aquí.
    create_screens();
    
    if (g_main_screen_obj) {
        ui_connect_actions();
        lv_obj_add_event_cb(g_main_screen_obj, (lv_event_cb_t)delete_screen_main, LV_EVENT_DELETE, NULL);
    }
    
    lv_screen_load(g_main_screen_obj);
    ESP_LOGI(TAG, "UI modularizada y lista.");
}
