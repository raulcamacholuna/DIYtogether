/* Fecha: 18/08/2025 - 10:00  */
/* Fichero: components/ui/actions/action_config_mode.c */
/* Último cambio: Añadido feedback visual al botón de reiniciar. */
/* Descripción: Se ha añadido una llamada al módulo 'button_feedback' para que el botón 'REINICIAR' del modo de configuración proporcione una respuesta visual al ser presionado, mejorando la experiencia de usuario y manteniendo la coherencia con el resto de los botones de la aplicación. */

#include "actions/action_config_mode.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_lvgl_port.h"
#include "bsp_api.h"
#include "web_server.h"
#include "screens.h"
#include "core/state_manager.h"
#include "esp_system.h"
#include "buttons/button_feedback.h" // Incluir el nuevo módulo de feedback

static const char *TAG = "ACTION_CONFIG_MODE";

// --- Variables estáticas privadas del módulo ---
static bool s_is_config_mode_active = false;
static lv_obj_t *s_config_screen = NULL;
static TaskHandle_t s_wifi_task_handle = NULL;
static httpd_handle_t s_server_handle = NULL;

// --- Funciones de ayuda privadas del módulo ---

/**
 * @brief Tarea de FreeRTOS para gestionar el modo de configuración.
 */
static void wifi_config_task(void *param) {
    bsp_wifi_init_stack();

    ESP_LOGI(TAG, "Intentando conectar a WiFi guardado...");
    lvgl_port_lock(0);
    if (s_is_config_mode_active && s_config_screen) {
        lv_obj_t *label1 = lv_obj_get_child(s_config_screen, 0);
        lv_label_set_text(label1, "Conectando a WiFi...");
        lv_obj_t *label2 = lv_obj_get_child(s_config_screen, 1);
        lv_label_set_text(label2, "Esperando IP...");
    }
    lvgl_port_unlock();

    bsp_wifi_init_sta_from_nvs();

    bool connected = bsp_wifi_wait_for_ip(10000);

    if (connected && s_is_config_mode_active) {
        char ip_addr[16];
        bsp_wifi_get_ip(ip_addr);
        ESP_LOGI(TAG, "Conectado exitosamente. IP: %s", ip_addr);

        lvgl_port_lock(0);
        if (s_is_config_mode_active && s_config_screen) {
            lv_obj_t *label1 = lv_obj_get_child(s_config_screen, 0);
            lv_obj_t *label2 = lv_obj_get_child(s_config_screen, 1);
            lv_label_set_text(label1, "¡Conectado!");
            lv_label_set_text_fmt(label2, "IP: %s\nAccede desde tu navegador", ip_addr);
        }
        lvgl_port_unlock();

    } else if (s_is_config_mode_active) {
        ESP_LOGI(TAG, "No se pudo conectar como STA. Iniciando modo AP.");
        bsp_wifi_deinit();

        lvgl_port_lock(0);
        if (s_is_config_mode_active && s_config_screen) {
            lv_obj_t *label1 = lv_obj_get_child(s_config_screen, 0);
            lv_obj_t *label2 = lv_obj_get_child(s_config_screen, 1);
            lv_label_set_text(label1, "Modo AP Activo");
            lv_label_set_text(label2, "SSID: DIYTogether\nPass: MakeItYours\nIP: 192.168.4.1");
        }
        lvgl_port_unlock();

        bsp_wifi_start_ap();
    }

    if (s_is_config_mode_active) {
        ESP_LOGI(TAG, "Iniciando servidor web para gestión.");
        s_server_handle = web_server_start();
    }

    s_wifi_task_handle = NULL;
    vTaskDelete(NULL);
}


/**
 * @brief Callback de evento para el botón 'Reiniciar' del modo de configuración.
 */
static void restart_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGW(TAG, "Botón 'Reiniciar' presionado. Reiniciando dispositivo...");
        vTaskDelay(pdMS_TO_TICKS(500)); // Pequeña pausa para enviar el log
        esp_restart();
    }
}

// --- Implementación de funciones públicas ---

void action_config_mode_start(void) {
    if (s_is_config_mode_active) return;
    s_is_config_mode_active = true;
    ESP_LOGI(TAG, "Entrando en modo de configuración: Liberando recursos de la UI principal.");

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
    lv_label_set_text(label1, "Iniciando WiFi...");
    
    lv_obj_set_style_text_color(label2, lv_color_white(), 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(label2, lv_pct(90));
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label2, "");

    lv_obj_t* restart_button = lv_btn_create(s_config_screen);
    lv_obj_align(restart_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(restart_button, restart_button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl = lv_label_create(restart_button);
    lv_label_set_text(lbl, "REINICIAR");
    lv_obj_center(lbl);

    // Añadir feedback visual al botón de reiniciar
    button_feedback_add(restart_button);

    lv_screen_load(s_config_screen);
    lvgl_port_unlock();

    xTaskCreate(wifi_config_task, "wifi_cfg_task", 4096, NULL, 5, &s_wifi_task_handle);
}
