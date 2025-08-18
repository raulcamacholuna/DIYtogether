/* Fecha: 18/08/2025 - 09:08  */
/* Fichero: components/ui/actions/action_config_mode.c */
/* Último cambio: Implementada la lógica para intentar conexión STA antes de iniciar el modo AP. */
/* Descripción: Se ha corregido el comportamiento del modo de configuración. Ahora, al entrar, primero intenta conectarse a una red WiFi guardada. Si la conexión es exitosa, muestra la IP obtenida. Si falla o no hay credenciales, limpia el intento de conexión STA y luego inicia el modo Punto de Acceso (AP), resolviendo el problema de que siempre iniciaba en modo AP. */

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
#include "core/ui.h" // Necesario para ui_init y ui_preinit

static const char *TAG = "ACTION_CONFIG_MODE";

// --- Variables estáticas privadas del módulo ---
static bool s_is_config_mode_active = false;
static lv_obj_t *s_config_screen = NULL;
static TaskHandle_t s_wifi_task_handle = NULL;
static httpd_handle_t s_server_handle = NULL; // Handle para el servidor web

// --- Funciones de ayuda privadas del módulo ---

/**
 * @brief Tarea de FreeRTOS para gestionar el modo de configuración.
 *        Intenta conectar como STA si hay credenciales; si no, inicia un AP.
 */
static void wifi_config_task(void *param) {
    bsp_wifi_init_stack();

    // 1. Intentar conectar como estación (STA) usando credenciales guardadas.
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

    // 2. Esperar a obtener una IP.
    bool connected = bsp_wifi_wait_for_ip(10000); // Timeout de 10 segundos

    if (connected && s_is_config_mode_active) {
        // 3a. Conexión exitosa: Mostrar IP y lanzar servidor.
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
        // 3b. Fallo en conexión o sin credenciales: Iniciar modo AP.
        ESP_LOGI(TAG, "No se pudo conectar como STA. Iniciando modo AP.");
        bsp_wifi_deinit(); // Limpiar el intento de STA fallido.

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

    // 4. Iniciar el servidor web en cualquier caso (STA o AP).
    if (s_is_config_mode_active) {
        ESP_LOGI(TAG, "Iniciando servidor web para gestión.");
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
    ESP_LOGI(TAG, "Entrando en modo de configuración: Liberando recursos de la UI principal.");

    // [OPTIMIZACIÓN] Destruir la UI principal para liberar memoria
    // La tarea de telemetría se detendrá por sí misma al no poder actualizar la UI.
    lvgl_port_lock(0);
    state_manager_destroy();
    delete_screen_main();
    lvgl_port_unlock();
    
    // Crear una pantalla simple para el modo config
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
    
    ESP_LOGI(TAG, "Saliendo del modo de configuración y restaurando UI.");
    s_is_config_mode_active = false;

    // 1. Detener todos los servicios de red y liberar sus recursos.
    web_server_stop(s_server_handle);
    s_server_handle = NULL;
    if (s_wifi_task_handle) { vTaskDelete(s_wifi_task_handle); s_wifi_task_handle = NULL; }
    
    // [CORRECCIÓN CRÍTICA] Llamar a la función de desinicialización completa del BSP.
    bsp_wifi_deinit();
    
    ESP_LOGI(TAG, "Servicios de red detenidos y limpiados.");
    
    lvgl_port_lock(0);
    // 2. Liberar la UI del modo configuración.
    if (s_config_screen) {
        lv_obj_del(s_config_screen);
        s_config_screen = NULL;
    }

    // 3. Pre-alocar el buffer de animación AHORA, mientras la memoria está limpia.
    ui_preinit();

    // 4. Reconstruir la UI principal y sus gestores.
    ui_init();
    state_manager_init();
    lvgl_port_unlock();

    // 5. Reiniciar tareas de la aplicación.
    telemetry_task_start();
    ESP_LOGI(TAG, "UI principal restaurada. Sistema de vuelta a modo normal.");
}
