/* Fecha: 16/08/2025 - 07:42  */
/* Fichero: components/diymon_ui/actions.c */
/* Último cambio: Corregidos errores de compilación por includes y variables faltantes. */
/* Descripción: Orquestador central de acciones. Llama a la función correspondiente del módulo adecuado basado en la acción del usuario. La nueva acción para activar el modo de configuración establece una bandera en NVS y reinicia el dispositivo. */

#include "actions.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "lvgl.h"
#include "esp_lvgl_port.h" // Necesario para lvgl_port_lock/unlock

// Includes de módulos con los que interactúa
#include "diymon_evolution.h"
#include "screen_manager.h"
#include "wifi_portal.h"
#include "ui_action_animations.h"
#include "ui_idle_animation.h"
#include "ui_telemetry.h"
#include "screens.h" 
#include "ui_actions_panel.h"
#include "bsp_api.h"

static const char *TAG = "DIYMON_ACTIONS";

// --- Variables estáticas para la gestión del brillo ---
static int s_brightness_levels[] = {25, 50, 75, 100};
static int s_current_brightness_idx = 3;

// --- Variables para el modo de configuración en tiempo real ---
static bool s_is_config_mode_active = false;
static lv_obj_t *s_config_status_label1 = NULL;
static lv_obj_t *s_config_status_label2 = NULL;
static lv_obj_t *s_config_back_button = NULL;
static TaskHandle_t s_wifi_task_handle = NULL;

extern lv_obj_t *g_animation_img_obj;

// --- Declaraciones de funciones ---
static void wifi_config_task(void *param);
static void config_back_button_event_cb(lv_event_t *e);

/**
 * @brief Tarea en segundo plano para gestionar la conexión WiFi sin bloquear la UI.
 */
static void wifi_config_task(void *param) {
    lvgl_port_lock(0);
    if (s_is_config_mode_active && s_config_status_label1) {
        lv_label_set_text(s_config_status_label1, "Conectando...");
        lv_label_set_text(s_config_status_label2, "Usando credenciales guardadas.");
    }
    lvgl_port_unlock();

    bsp_wifi_init_stack();
    bsp_wifi_init_sta_from_nvs();
    bool ip_ok = bsp_wifi_wait_for_ip(15000);

    if (!ip_ok) {
        bsp_wifi_start_ap();
    }
    
    lvgl_port_lock(0);
    if (s_is_config_mode_active) {
        char ip_addr_buffer[16] = "192.168.4.1";
        if (ip_ok) {
            bsp_wifi_get_ip(ip_addr_buffer);
            lv_label_set_text(s_config_status_label1, "Conectado!");
            lv_label_set_text_fmt(s_config_status_label2, "IP: %s", ip_addr_buffer);
        } else {
            lv_label_set_text(s_config_status_label1, "Modo AP Activo");
            lv_label_set_text(s_config_status_label2, "SSID: DIYTogether\nIP: 192.168.4.1");
        }
    }
    lvgl_port_unlock();

    s_wifi_task_handle = NULL;
    vTaskDelete(NULL);
}

static void config_back_button_event_cb(lv_event_t *e) {
    if (s_is_config_mode_active) {
        ESP_LOGI(TAG, "Saliendo del modo de configuración WiFi.");
        s_is_config_mode_active = false;

        if (s_wifi_task_handle) { vTaskDelete(s_wifi_task_handle); s_wifi_task_handle = NULL; }
        
        esp_wifi_stop();
        esp_wifi_deinit();

        if (s_config_status_label1) { lv_obj_del(s_config_status_label1); s_config_status_label1 = NULL; }
        if (s_config_status_label2) { lv_obj_del(s_config_status_label2); s_config_status_label2 = NULL; }
        if (s_config_back_button) { lv_obj_del(s_config_back_button); s_config_back_button = NULL; }

        if(g_animation_img_obj) lv_obj_clear_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
        ui_telemetry_create(g_main_screen_obj);
        ui_idle_animation_start(g_main_screen_obj);
    }
}

static void erase_nvs_key(const char* key) {
    nvs_handle_t nvs_handle;
    if (nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
        nvs_erase_key(nvs_handle, key);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

void execute_diymon_action(diymon_action_id_t action_id) {
    ESP_LOGI(TAG, "Ejecutando acción ID: %d", action_id);

    if (action_id == ACTION_ID_ACTIVATE_CONFIG_MODE) {
        if (s_is_config_mode_active) return;
        s_is_config_mode_active = true;
        ESP_LOGI(TAG, "Entrando en modo de configuración WiFi sin reinicio.");

        ui_actions_panel_hide_all();
        ui_idle_animation_stop();
        ui_telemetry_destroy();
        if (g_animation_img_obj) lv_obj_add_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);

        s_config_status_label1 = lv_label_create(g_main_screen_obj);
        s_config_status_label2 = lv_label_create(g_main_screen_obj);
        lv_obj_set_style_text_color(s_config_status_label1, lv_color_white(), 0);
        lv_obj_set_style_text_align(s_config_status_label1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(s_config_status_label1, lv_pct(90));
        lv_obj_align(s_config_status_label1, LV_ALIGN_CENTER, 0, -40);
        lv_obj_set_style_text_color(s_config_status_label2, lv_color_white(), 0);
        lv_obj_set_style_text_align(s_config_status_label2, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(s_config_status_label2, lv_pct(90));
        lv_obj_align(s_config_status_label2, LV_ALIGN_CENTER, 0, 0);

        s_config_back_button = lv_btn_create(g_main_screen_obj);
        lv_obj_align(s_config_back_button, LV_ALIGN_BOTTOM_MID, 0, -20);
        lv_obj_add_event_cb(s_config_back_button, config_back_button_event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_t* lbl = lv_label_create(s_config_back_button);
        lv_label_set_text(lbl, "VOLVER");
        lv_obj_center(lbl);

        xTaskCreate(wifi_config_task, "wifi_cfg_task", 4096, NULL, 5, &s_wifi_task_handle);
        return;
    }

    if (s_is_config_mode_active) {
        ESP_LOGW(TAG, "Acción %d ignorada, modo de configuración activo.", action_id);
        return;
    }

    switch(action_id) {
        case ACTION_ID_COMER: case ACTION_ID_EJERCICIO: case ACTION_ID_ATACAR:
            ui_action_animations_play(action_id);
            break;
        case ACTION_ID_BRIGHTNESS_CYCLE:
            s_current_brightness_idx = (s_current_brightness_idx + 1) % 4;
            screen_manager_set_brightness(s_brightness_levels[s_current_brightness_idx]);
            break;
        case ACTION_ID_TOGGLE_SCREEN:
            if (screen_manager_is_off()) screen_manager_turn_on();
            else { ui_actions_panel_hide_all(); screen_manager_turn_off(); }
            break;
        case ACTION_ID_ENABLE_FILE_SERVER: {
            nvs_handle_t nvs_handle;
            if (nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
                nvs_set_str(nvs_handle, "file_server", "1"); nvs_commit(nvs_handle); nvs_close(nvs_handle);
            }
            vTaskDelay(pdMS_TO_TICKS(500)); esp_restart();
            break;
        }
        case ACTION_ID_RESET_ALL:
            wifi_portal_erase_credentials(); diymon_evolution_reset_state(); erase_nvs_key("file_server");
            vTaskDelay(pdMS_TO_TICKS(1000)); esp_restart();
            break;
        case ACTION_ID_EVO_FIRE: case ACTION_ID_EVO_WATER: case ACTION_ID_EVO_EARTH: case ACTION_ID_EVO_WIND: {
            const char* code = diymon_get_current_code();
            int branch = (action_id - ACTION_ID_EVO_FIRE) + 1;
            const char* next = diymon_get_branched_evolution(code, branch);
            if (next) {
                ui_idle_animation_stop(); if (g_animation_img_obj) lv_obj_add_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
                diymon_set_current_code(next); ui_telemetry_force_update();
                ui_idle_animation_start(g_main_screen_obj); if (g_animation_img_obj) lv_obj_clear_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        }
        case ACTION_ID_EVO_BACK: {
            const char* code = diymon_get_current_code();
            const char* prev = diymon_get_previous_evolution_in_sequence(code);
            if (prev) {
                ui_idle_animation_stop(); if (g_animation_img_obj) lv_obj_add_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
                diymon_set_current_code(prev); ui_telemetry_force_update();
                ui_idle_animation_start(g_main_screen_obj); if (g_animation_img_obj) lv_obj_clear_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        }
        default:
            ESP_LOGW(TAG, "ID de accion desconocido: %d", action_id);
            break;
    }
}
