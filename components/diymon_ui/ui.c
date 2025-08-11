/*
  Fichero: ./components/diymon_ui/ui.c
  Fecha: 11/08/2025 - 22:00
  Último cambio: Corregida la conexión de eventos para el botón de FTP.
  Descripción: Se actualiza el callback del botón del panel de administración
               para usar el getter y el ID de acción correctos para la
               funcionalidad de habilitar el modo FTP.
*/
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_actions_panel.h"
#include "ui_asset_loader.h"
#include "esp_log.h"

extern lv_obj_t *g_main_screen_obj; 
extern lv_obj_t *g_idle_animation_obj; 

static const char *TAG = "DIYMON_UI_MAIN";

static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED) {
        ESP_LOGI(TAG, "Evento de botón detectado para la acción ID: %d", action_id);
        execute_diymon_action(action_id, g_idle_animation_obj);
    }
}

static void ui_connect_actions(void) {
    // Conectar acciones del panel de jugador
    lv_obj_add_event_cb(ui_actions_panel_get_eat_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_COMER);
    lv_obj_add_event_cb(ui_actions_panel_get_gym_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EJERCICIO);
    lv_obj_add_event_cb(ui_actions_panel_get_atk_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_ATACAR);

    // Conectar acciones del panel de admin
    lv_obj_add_event_cb(ui_actions_panel_get_brightness_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_BRIGHTNESS_CYCLE);
    lv_obj_add_event_cb(ui_actions_panel_get_toggle_screen_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_TOGGLE_SCREEN);
    lv_obj_add_event_cb(ui_actions_panel_get_enable_ftp_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_ENABLE_FTP);
    
    // Conectar acciones del panel lateral de evolución
    lv_obj_add_event_cb(ui_actions_panel_get_evo_fire_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_FIRE);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_water_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_WATER);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_earth_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_EARTH);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_wind_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_WIND);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_back_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_BACK);

    ESP_LOGI(TAG, "Eventos de todos los botones de acción conectados.");
}

void ui_init(void) {
    ui_assets_init();
    create_screens();
    
    if (g_main_screen_obj) {
        ui_connect_actions();
        lv_obj_add_event_cb(g_main_screen_obj, (lv_event_cb_t)delete_screen_main, LV_EVENT_DELETE, NULL);
    }
    
    lv_screen_load(g_main_screen_obj);
    ESP_LOGI(TAG, "UI modularizada y lista.");
}