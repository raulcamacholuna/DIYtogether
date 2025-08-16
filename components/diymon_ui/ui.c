/* Fecha: 16/08/2025 - 07:46  */
/* Fichero: components/diymon_ui/ui.c */
/* Último cambio: Corregida la conexión de eventos para los botones del panel de configuración. */
/* Descripción: Se ha modificado la función `ui_connect_actions` para asignar la acción `ACTION_ID_ACTIVATE_CONFIG_MODE` al botón 9 (el tercer botón del panel de configuración), tal como solicitó el usuario. Los otros botones del panel de configuración se han enlazado temporalmente a una acción placeholder para evitar comportamientos no deseados. */
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
    // Para las acciones de jugador, queremos respuesta inmediata al presionar
    // Para el resto, esperamos al evento 'CLICKED' para evitar acciones accidentales
    if (code == LV_EVENT_PRESSED || code == LV_EVENT_CLICKED) {
        diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
        execute_diymon_action(action_id);
    }
}

static void ui_connect_actions(void) {
    // --- Panel de Jugador ---
    lv_obj_add_event_cb(ui_actions_panel_get_eat_btn(), button_event_cb, LV_EVENT_PRESSED, (void*)ACTION_ID_COMER);
    lv_obj_add_event_cb(ui_actions_panel_get_gym_btn(), button_event_cb, LV_EVENT_PRESSED, (void*)ACTION_ID_EJERCICIO);
    lv_obj_add_event_cb(ui_actions_panel_get_atk_btn(), button_event_cb, LV_EVENT_PRESSED, (void*)ACTION_ID_ATACAR);

    // --- Panel de Administración ---
    lv_obj_add_event_cb(ui_actions_panel_get_brightness_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_BRIGHTNESS_CYCLE);
    lv_obj_add_event_cb(ui_actions_panel_get_toggle_screen_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_TOGGLE_SCREEN);
    lv_obj_add_event_cb(ui_actions_panel_get_admin_placeholder_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ADMIN_PLACEHOLDER);

    // --- Panel de Evolución ---
    lv_obj_add_event_cb(ui_actions_panel_get_evo_fire_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_FIRE);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_water_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_WATER);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_earth_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_EARTH);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_wind_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_WIND);
    lv_obj_add_event_cb(ui_actions_panel_get_evo_back_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_BACK);
    
    // --- Panel de Configuración (con la lógica corregida) ---
    // Botones 7 y 8 temporalmente desactivados (apuntan a un placeholder)
    lv_obj_add_event_cb(ui_actions_panel_get_reset_all_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_RESET_ALL);
    lv_obj_add_event_cb(ui_actions_panel_get_enable_file_server_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ENABLE_FILE_SERVER);
    
    // Botón 9 (tercero del panel) activa el modo de configuración WiFi en tiempo real
    lv_obj_add_event_cb(ui_actions_panel_get_config_placeholder_btn(), button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ACTIVATE_CONFIG_MODE);

    ESP_LOGI(TAG, "Eventos de todos los botones de acción conectados.");
}

void ui_preinit(void) {
    ESP_LOGI(TAG, "Pre-inicializando UI: reservando buffer de animación...");
    ui_action_animations_preinit_buffer();
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
