/* Fichero: components/ui/actions/action_config_mode.c */
/* Descripción: Diagnóstico de Causa Raíz: El 'Load access fault' es un error de tipo 'use-after-free' que ocurre por ejecutar una destrucción compleja de la UI de forma síncrona dentro de un callback de evento de LVGL. La llamada a lv_obj_del() marca la pantalla y sus hijos para ser borrados, pero el proceso de limpieza real se completa más tarde en el ciclo principal de LVGL. Al ejecutar la destrucción de forma síncrona, se crea una condición de carrera en la que el manejador de LVGL intenta acceder a objetos que ya han sido invalidados en el mismo ciclo de eventos, causando el crash.
Solución Definitiva: Toda la lógica de transición al modo de configuración se ha encapsulado en una única función (	ransition_to_config_mode_cb) que es invocada de forma asíncrona a través de un temporizador de un solo uso. La función ction_config_mode_start ahora solo se encarga de programar este temporizador. Esto garantiza que el ciclo de eventos actual de LVGL finalice de forma limpia. La transición (destrucción de la UI antigua y creación de la nueva) se ejecuta en un ciclo posterior, eliminando la condición de carrera y asegurando la estabilidad del estado de LVGL. */
/* Último cambio: 20/08/2025 - 07:56 */
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
#include "buttons/button_feedback.h"
#include "telemetry/telemetry_task.h"

static const char *TAG = "ACTION_CONFIG_MODE";

// --- Variables estáticas privadas del módulo ---
static bool s_is_config_mode_active = false;
static lv_obj_t *s_config_screen = NULL;
static TaskHandle_t s_wifi_task_handle = NULL;
static httpd_handle_t s_server_handle = NULL;

// --- Funciones de ayuda privadas del módulo ---

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

static void restart_button_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ESP_LOGW(TAG, "Botón 'Reiniciar' presionado. Reiniciando dispositivo...");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    }
}

static void transition_to_config_mode_cb(lv_timer_t *timer) {
    ESP_LOGI(TAG, "Ejecutando transición al modo configuración...");

    // 1. Detener tareas y gestores que interactúan con la UI
    telemetry_task_stop();
    state_manager_destroy();

    // 2. Destruir la pantalla principal
    delete_screen_main();

    // 3. Crear y mostrar la nueva pantalla de configuración
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

    button_feedback_add(restart_button);
    lv_screen_load(s_config_screen);
    lvgl_port_unlock();

    // 4. Iniciar la tarea de gestión de WiFi
    xTaskCreate(wifi_config_task, "wifi_cfg_task", 4096, NULL, 5, &s_wifi_task_handle);
}

// --- Implementación de funciones públicas ---

void action_config_mode_start(void) {
    if (s_is_config_mode_active) return;
    s_is_config_mode_active = true;
    ESP_LOGI(TAG, "Programando transición al modo de configuración...");

    // Se programa un temporizador para ejecutar la transición de forma asíncrona.
    // Esto permite que el ciclo de eventos actual de LVGL termine limpiamente.
    lv_timer_create(transition_to_config_mode_cb, 20, NULL)->repeat_count = 1;
}
