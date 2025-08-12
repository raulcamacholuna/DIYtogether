/*
  Fichero: ./components/diymon_ui/ui.c
  Fecha: 14/08/2025 - 10:30 am
  Último cambio: Corregida la llamada a `execute_diymon_action` para alinearla con la nueva firma.
  Descripción: Orquestador principal de la UI. Se elimina el segundo parámetro en la llamada a la función de ejecución de acciones, completando así la refactorización a un búfer de animación compartido.
*/
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_actions_panel.h"
#include "ui_asset_loader.h"
#include "esp_log.h"

// El objeto de la pantalla principal se define en screens.c y se declara aquí para su uso.
extern lv_obj_t *g_main_screen_obj; 

static const char *TAG = "DIYMON_UI_MAIN";

static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED) {
        ESP_LOGI(TAG, "Evento de botón detectado para la acción ID: %d", action_id);
        // Llamada corregida con un solo argumento, como requiere la nueva firma.
        execute_diymon_action(action_id);
    }
}

static void ui_connect_actions(void) {
    // Panel 1: Jugador
    lv_obj_add_event_cb(ui_actions_panel_get_eat_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_COMER);
    lv_obj_add_event_cb(ui_actions_panel_get_gym_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_EJERCICIO);
    lv_obj_add_event_cb(ui_actions_panel_get_atk_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_ATACAR);

    // Panel 2: Admin
    lv_obj_add_event_cb(ui_actions_panel_get_brightness_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_BRIGHTNESS_CYCLE);
    lv_obj_add_event_cb(ui_actions_panel_get_toggle_screen_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_TOGGLE_SCREEN);
    lv_obj_add_event_cb(ui_actions_panel_get_admin_placeholder_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_ADMIN_PLACEHOLDER);
    
    // Panel 3: Configuración
    lv_obj_add_event_cb(ui_actions_panel_get_reset_all_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_RESET_ALL);
    lv_obj_add_event_cb(ui_actions_panel_get_enable_ftp_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_ENABLE_FTP);
    lv_obj_add_event_cb(ui_actions_panel_get_config_placeholder_btn(), button_event_cb, LV_EVENT_ALL, (void*)ACTION_ID_CONFIG_PLACEHOLDER);
    
    // Panel Lateral: Evolución
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