/* Fecha: 18/08/2025 - 08:33  */
/* Fichero: components/ui/actions/action_config_mode.c */
/* Último cambio: Iniciado el servidor web también en modo STA para permitir el acceso a la configuración a través de la red local. */
/* Descripción: Se ha corregido la lógica para que el servidor web se inicie independientemente del modo WiFi. Ahora, si el dispositivo se conecta a una red existente (modo STA), el servidor web también se activará. Esto permite acceder a la página de configuración a través de la IP local asignada, solucionando el error 'ERR_CONNECTION_REFUSED'. */

#include "actions/action_config_mode.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_lvgl_port.h"
#include "bsp_api.h"
#include "web_server.h"
#include "telemetry/telemetry_task.h"
#include "telemetry/telemetry_manager.h"
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "core/state_manager.h"
#include "core/ui.h"

static const char *TAG = "ACTION_CONFIG_MODE";

// --- Variables estáticas privadas del módulo ---
static bool s_is_config_mode_active = false;
static lv_obj_t *s_config_screen = NULL;
static TaskHandle_t s_wifi_task_handle = NULL;
static httpd_handle_t s_server_handle = NULL;
static bool s_ap_mode_was_activated = false;

/**
 * @brief Tarea de FreeRTOS para gestionar el modo de configuración.
 *        Intenta conectar como STA, si falla, inicia el modo AP.
 */
static void wifi_config_task(void *param) {
    bsp_wifi_init_stack();
    
    bsp_wifi_init_sta_from_nvs();
    bool is_connected = bsp_wifi_wait_for_ip(10000);

    lvgl_port_lock(0);
    if (!s_is_config_mode_active) {
        lvgl_port_unlock();
        bsp_wifi_deinit();
        vTaskDelete(NULL);
        return;
    }
    
    lv_obj_t *label1 = lv_obj_get_child(s_config_screen, 0);
    lv_obj_t *label2 = lv_obj_get_child(s_config_screen, 1);

    if (is_connected) {
        ESP_LOGI(TAG, "Conexión WiFi exitosa. Mostrando información.");
        char ip_str[16] = {0};
        bsp_wifi_get_ip(ip_str);
        lv_label_set_text(label1, "Conectado a WiFi");
        lv_label_set_text_fmt(label2, "IP: %s", ip_str);
        s_ap_mode_was_activated = false;
    } else {
        ESP_LOGW(TAG, "No se pudo conectar a WiFi. Iniciando modo AP.");
        bsp_wifi_deinit();
        bsp_wifi_init_stack();
        bsp_wifi_start_ap();
        lv_label_set_text(label1, "Modo AP Activo");
        lv_label_set_text(label2, "SSID: DIYTogether\nPass: MakeItYours\nIP: 192.168.4.1");
        s_ap_mode_was_activated = true;
    }
    lvgl_port_unlock();

    // [CORRECCIÓN] Iniciar el servidor web en AMBOS casos (STA o AP).
    if (s_is_config_mode_active) {
        ESP_LOGI(TAG, "Iniciando servidor web.");
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

void action_config_mode_start(void) {
    if (s_is_config_mode_active) return;
    s_is_config_mode_active = true;
    s_ap_mode_was_activated = false;
    ESP_LOGI(TAG, "Entrando en modo de configuración: Liberando UI y activando WiFi...");

    lvgl_port_lock(0);
    state_manager_destroy();
    delete_screen_main();
    lvgl_port_unlock();
    
    lvgl_port_lock(0);
    s_config_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(s_config_screen, lv_color_black(), 0);

    lv_obj_t *label1 = lv_label_create(s_config_screen);
    lv_obj_t *label2 = lv_label_create(s_config_screen);
    lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(label1, lv_pct(90));
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);
    lv_label_set_text(label1, "Comprobando WiFi...");
    
    lv_obj_set_style_text_color(label2, lv_color_white(), 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(label2, lv_pct(90));
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label2, "");

    lv_obj_t* back_button = lv_btn_create(s_config_screen);
    lv_obj_align(back_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(back_button, config_back_button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl = lv_label_create(back_button);
    lv_label_set_text(lbl, "VOLVER");
    lv_obj_center(lbl);

    lv_screen_load(s_config_screen);
    lvgl_port_unlock();

    xTaskCreate(wifi_config_task, "wifi_cfg_task", 4096, NULL, 5, &s_wifi_task_handle);
}

void action_config_mode_stop(void) {
    if (!s_is_config_mode_active) return;
    
    ESP_LOGI(TAG, "Saliendo de modo de configuración: Desactivando WiFi y restaurando UI.");
    s_is_config_mode_active = false;

    if (s_wifi_task_handle) { 
        vTaskDelete(s_wifi_task_handle); 
        s_wifi_task_handle = NULL; 
    }

    if (s_server_handle) {
        web_server_stop(s_server_handle);
        s_server_handle = NULL;
    }
    bsp_wifi_deinit();

    lvgl_port_lock(0);
    if (s_config_screen) {
        lv_obj_del(s_config_screen);
        s_config_screen = NULL;
    }
    ui_init();
    state_manager_init();
    lvgl_port_unlock();

    telemetry_task_start();
}
