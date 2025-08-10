/*
  Fichero: ./components/diymon_ui/ui.c
  Fecha: 13/08/2025 - 09:10
  Último cambio: Conectados los eventos de los botones del panel lateral.
  Descripción: Se corrige el error de compilación por 'declaración implícita' añadiendo la cabecera del panel de acciones. Esto permite que el fichero conozca las funciones ui_actions_panel_get_..._btn y sus tipos de retorno correctos.
*/
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_actions_panel.h" // <-- LA LÍNEA QUE FALTABA
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
    lv_obj_add_event_cb(ui_actions_panel_get_lvl_down_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_LVL_DOWN);
    lv_obj_add_event_cb(ui_actions_panel_get_screen_off_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_SCREEN_OFF);
    lv_obj_add_event_cb(ui_actions_panel_get_lvl_up_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_LVL_UP);
    
    // Conectar acciones del panel lateral de evolución
    lv_obj_add_event_cb(ui_actions_panel_get_evo_fire_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_FIRE);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_water_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_WATER);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_earth_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_EARTH);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_wind_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_WIND);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_back_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EVO_BACK);

    ESP_LOGI(TAG, "Eventos de todos los botones de acción conectados.");
}

void ui_init(void) {
    // 1. Precargar todos los assets necesarios para la UI.
    ui_assets_init();

    // 2. Crear las pantallas, que ahora pueden usar los assets ya cargados.
    create_screens();
    
    if (g_main_screen_obj) {
        ui_connect_actions();
        lv_obj_add_event_cb(g_main_screen_obj, (lv_event_cb_t)delete_screen_main, LV_EVENT_DELETE, NULL);
    }
    
    lv_screen_load(g_main_screen_obj);
    ESP_LOGI(TAG, "UI modularizada y lista.");
}