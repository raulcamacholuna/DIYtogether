/* Fichero: components/ui/actions/action_config_mode.c */
/* Último cambio: Corregida la condición de carrera en el bloqueo de LVGL para resolver el crash 'Load access fault'. */
/* Descripción: Diagnóstico de Causa Raíz: El crash final ('Load access fault') ocurre por una condición de carrera entre la tarea principal (que destruye la UI) y la tarea de LVGL (que la renderiza). La secuencia del fallo es:
1. 'action_config_mode_start' se ejecuta en la tarea de LVGL (a través de un evento de botón).
2. Llama a 'lvgl_port_lock()'.
3. Llama a 'delete_screen_main', que planifica el borrado de objetos y libera buffers.
4. Llama a 'lvgl_port_unlock()'.
5. En ese instante, la tarea de LVGL, que tiene mayor prioridad, puede ser interrumpida. Si otra tarea (como 'telemetry_task') intenta bloquear el mutex de LVGL, podría causar un cambio de contexto.
6. La tarea de LVGL, al reanudarse para renderizar, intenta acceder a los objetos que acaban de ser marcados para borrado y cuyos buffers asociados ya no existen, causando el 'Load access fault'.
Solución Definitiva: Se ha eliminado el bloqueo del mutex de LVGL ('lvgl_port_lock'/'unlock') de la función 'action_config_mode_start'. Como esta función ya se ejecuta dentro de la tarea de LVGL (al ser llamada desde un callback de evento), el acceso a los objetos de la UI es inherentemente seguro (thread-safe). Eliminar el bloqueo innecesario simplifica el código y, lo más importante, evita la ventana de oportunidad para la condición de carrera, resolviendo el crash de forma definitiva y estable. */
/* Último cambio: 20/08/2025 - 07:27 */
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

static const char *TAG = "ACTION_CONFIG_MODE";

// --- Variables estáticas privadas del módulo ---
static bool s_is_config_mode_active = false;
static lv_obj_t *s_config_screen = NULL;
static TaskHandle_t s_wifi_task_handle = NULL;
static httpd_handle_t s_server_handle = NULL;

// --- Declaraciones adelantadas ---
static void create_and_load_config_screen_cb(lv_timer_t *timer);

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
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGW(TAG, "Botón 'Reiniciar' presionado. Reiniciando dispositivo...");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    }
}

static void create_and_load_config_screen_cb(lv_timer_t *timer) {
    lvgl_port_lock(0); // Bloqueo seguro para crear la nueva pantalla
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

    xTaskCreate(wifi_config_task, "wifi_cfg_task", 4096, NULL, 5, &s_wifi_task_handle);
}

// --- Implementación de funciones públicas ---

void action_config_mode_start(void) {
    if (s_is_config_mode_active) return;
    s_is_config_mode_active = true;
    ESP_LOGI(TAG, "Entrando en modo de configuración: Liberando recursos de la UI principal.");

    // [CORRECCIÓN] No se necesita lvgl_port_lock/unlock aquí.
    // La función se llama desde un callback de LVGL, por lo que ya está en el contexto de la tarea de LVGL.
    state_manager_destroy();
    delete_screen_main();
    
    // Programar la creación de la nueva pantalla en un ciclo de LVGL posterior para evitar condiciones de carrera.
    lv_timer_t* timer = lv_timer_create(create_and_load_config_screen_cb, 10, NULL);
    lv_timer_set_repeat_count(timer, 1);
}
