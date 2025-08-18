/* Fecha: 18/08/2025 - 07:07  */
/* Fichero: components/ui/actions/action_config_mode.c */
/* Último cambio: Implementado el apagado controlado del servidor web para resolver el error de socket. */
/* Descripción: Se ha modificado el ciclo de vida del modo de configuración para que guarde el handle del servidor web al iniciarlo y llame explícitamente a 'web_server_stop' antes de detener la WiFi. Este cambio de secuencia es crucial y soluciona el error 'httpd_sock_err: error in send : 11', que ocurría porque se intentaba usar un socket después de que la interfaz de red subyacente hubiera sido deshabilitada. */

#include "actions/action_config_mode.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_lvgl_port.h"
#include "bsp_api.h"
#include "web_server.h"
#include "telemetry/telemetry_manager.h"
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "core/state_manager.h"

static const char *TAG = "ACTION_CONFIG_MODE";

// --- Variables estáticas privadas del módulo ---
static bool s_is_config_mode_active = false;
static lv_obj_t *s_config_status_label1 = NULL;
static lv_obj_t *s_config_status_label2 = NULL;
static lv_obj_t *s_config_back_button = NULL;
static TaskHandle_t s_wifi_task_handle = NULL;
static httpd_handle_t s_server_handle = NULL; // Handle para el servidor web

// --- Funciones de ayuda privadas del módulo ---

/**
 * @brief Tarea de FreeRTOS para gestionar el modo de configuración (AP + Servidor Web).
 */
static void wifi_config_task(void *param) {
    bsp_wifi_init_stack();
    bsp_wifi_start_ap();
    
    lvgl_port_lock(0);
    if (s_is_config_mode_active) {
        lv_label_set_text(s_config_status_label1, "Modo AP Activo");
        lv_label_set_text(s_config_status_label2, "SSID: DIYTogether\nPass: MakeItYours\nIP: 192.168.4.1");
    }
    lvgl_port_unlock();

    if (s_is_config_mode_active) {
        ESP_LOGI(TAG, "Iniciando servidor web para gestión de archivos.");
        s_server_handle = web_server_start();
    }

    s_wifi_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Callback de evento para el botón 'Volver' del modo de configuración.
 */
static void config_back_button_event_cb(lv_event_t *e) {
    action_config_mode_stop();
}

// --- Implementación de funciones públicas ---

void action_config_mode_start(void) {
    if (s_is_config_mode_active) return;
    s_is_config_mode_active = true;
    ESP_LOGI(TAG, "Entrando en modo de configuración WiFi (AP + Servidor Web).");

    state_manager_pause();

    ui_actions_panel_hide_all();
    ui_idle_animation_stop();
    telemetry_manager_destroy();
    if (g_animation_img_obj) lv_obj_add_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
    
    vTaskDelay(pdMS_TO_TICKS(500));

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
}

void action_config_mode_stop(void) {
    if (s_is_config_mode_active) {
        ESP_LOGI(TAG, "Saliendo del modo de configuración WiFi.");
        s_is_config_mode_active = false;

        // [CORRECCIÓN CRÍTICA] Detener el servidor web ANTES de apagar la WiFi.
        web_server_stop(s_server_handle);
        s_server_handle = NULL;

        if (s_wifi_task_handle) { vTaskDelete(s_wifi_task_handle); s_wifi_task_handle = NULL; }
        
        esp_wifi_stop();
        esp_wifi_deinit();

        esp_netif_t* ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
        if (ap_netif) { esp_netif_destroy(ap_netif); }
        
        if (s_config_status_label1) { lv_obj_del(s_config_status_label1); s_config_status_label1 = NULL; }
        if (s_config_status_label2) { lv_obj_del(s_config_status_label2); s_config_status_label2 = NULL; }
        if (s_config_back_button) { lv_obj_del(s_config_back_button); s_config_back_button = NULL; }

        if(g_animation_img_obj) lv_obj_clear_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
        telemetry_manager_create(g_main_screen_obj);
        ui_idle_animation_start(g_main_screen_obj);
        
        state_manager_resume();
    }
}
