/*
Fichero: Z:\DIYTOGETHER\DIYtogether\main\main.c
Fecha: $timestamp
Último cambio: Implementada secuencia de despertar con doble-doble-toque.
Descripción: Orquestador principal de la aplicación. Se ha implementado una nueva lógica de despertar de pantalla que requiere una secuencia de dos dobles-toques para encenderla desde el estado apagado, evitando activaciones accidentales. Se ha centralizado esta lógica en main.c, y se ha ajustado para que un solo toque siga restaurando el brillo si la pantalla está solo atenuada.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "esp_lvgl_port.h"

#include "bsp_api.h"
#include "hardware_manager.h"
#include "diymon_evolution.h"
#include "ui.h"
#include "wifi_portal.h"
#include "web_server.h"
#include "screen_manager.h"
#include "ui_config_screen.h"
#include "ui_asset_loader.h" 

#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_file_server_mode(void);
static void run_wifi_portal_mode(void);
static void run_main_application_mode(void);
static bool check_file_server_mode_flag(void);
static void erase_file_server_mode_flag(void);
static void init_lvgl_for_service_screen(void);
static void display_network_status_on_screen(bool is_connected, const char* ip_addr);
static void setup_inactivity_handling(void);

// --- Lógica de Gestión de Inactividad y Despertar ---
typedef enum {
    WAKE_STATE_OFF,
    WAKE_STATE_PRIMED, // Esperando el segundo doble-toque
} wake_up_state_t;

static bool s_is_dimmed = false;
static int s_user_brightness = 100;
static bool s_user_brightness_known = false;

static wake_up_state_t s_wake_state = WAKE_STATE_OFF;
static uint8_t s_wake_click_count = 0;
static lv_timer_t *s_double_click_timer = NULL;
static lv_timer_t *s_wake_prime_timer = NULL;


static void read_user_brightness_from_nvs(void) {
    if (s_user_brightness_known) return;
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        nvs_get_i32(nvs_handle, "brightness", (int32_t*)&s_user_brightness);
        nvs_close(nvs_handle);
        s_user_brightness_known = true;
        ESP_LOGI(TAG, "Brillo de usuario por defecto guardado: %d%%", s_user_brightness);
    }
}

static void double_click_timer_cb(lv_timer_t * timer) {
    ESP_LOGD(TAG, "Temporizador de doble-toque expirado. Reiniciando contador de toques.");
    s_wake_click_count = 0;
    s_double_click_timer = NULL;
}

static void wake_prime_timer_cb(lv_timer_t * timer) {
    ESP_LOGI(TAG, "Ventana de 3s para segundo doble-toque expirada. Secuencia de despertar cancelada.");
    s_wake_state = WAKE_STATE_OFF;
    s_wake_click_count = 0;
    s_wake_prime_timer = NULL;
}

static void screen_touch_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;

    // Cualquier tipo de presión resetea el temporizador de inactividad de LVGL
    lv_display_trigger_activity(disp);
    
    // Si la pantalla está atenuada pero no apagada, un solo toque la restaura
    if (code == LV_EVENT_PRESSED && s_is_dimmed && !screen_manager_is_off()) {
        ESP_LOGI(TAG, "Actividad detectada en pantalla atenuada, restaurando brillo.");
        read_user_brightness_from_nvs();
        screen_manager_set_brightness(s_user_brightness);
        s_is_dimmed = false;
        return; // Evento gestionado
    }

    // Lógica de despertar con doble-doble-toque SÓLO si la pantalla está apagada
    if (code == LV_EVENT_CLICKED && screen_manager_is_off()) {
        switch (s_wake_state) {
            case WAKE_STATE_OFF:
                s_wake_click_count++;
                if (s_wake_click_count == 1) { // Primer toque de un posible doble-toque
                    s_double_click_timer = lv_timer_create(double_click_timer_cb, 500, NULL);
                    lv_timer_set_repeat_count(s_double_click_timer, 1);
                } else if (s_wake_click_count == 2) { // Doble-toque detectado
                    if (s_double_click_timer) lv_timer_del(s_double_click_timer);
                    s_double_click_timer = NULL;
                    ESP_LOGI(TAG, "Secuencia de despertar: PRIMER doble-toque detectado. Esperando el segundo...");
                    s_wake_state = WAKE_STATE_PRIMED;
                    s_wake_click_count = 0;
                    s_wake_prime_timer = lv_timer_create(wake_prime_timer_cb, 3000, NULL);
                    lv_timer_set_repeat_count(s_wake_prime_timer, 1);
                }
                break;

            case WAKE_STATE_PRIMED: // Esperando el segundo doble-toque
                s_wake_click_count++;
                 if (s_wake_click_count == 1) {
                    s_double_click_timer = lv_timer_create(double_click_timer_cb, 500, NULL);
                    lv_timer_set_repeat_count(s_double_click_timer, 1);
                } else if (s_wake_click_count >= 2) { // Segundo doble-toque detectado
                    if (s_double_click_timer) lv_timer_del(s_double_click_timer);
                    if (s_wake_prime_timer) lv_timer_del(s_wake_prime_timer);
                    s_double_click_timer = NULL;
                    s_wake_prime_timer = NULL;
                    
                    ESP_LOGI(TAG, "Secuencia de despertar: SEGUNDO doble-toque detectado. ¡Encendiendo pantalla!");
                    screen_manager_turn_on(); // Enciende y restaura brillo
                    s_is_dimmed = false;
                    s_wake_state = WAKE_STATE_OFF;
                    s_wake_click_count = 0;
                }
                break;
        }
    }
}

static void inactivity_timer_cb(lv_timer_t * timer) {
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;

    uint32_t inactivity_ms = lv_display_get_inactive_time(disp);
    bool is_off = screen_manager_is_off();
    
    // Si el usuario interactúa, lv_display_send_activity resetea el contador,
    // por lo que este callback se ejecutará de nuevo y s_is_dimmed se pondrá a false.
    if (inactivity_ms < 30000 && s_is_dimmed) {
        s_is_dimmed = false;
    }

    if (!is_off && inactivity_ms > 60000) {
        ESP_LOGI(TAG, "Inactividad > 60s. Apagando pantalla.");
        screen_manager_turn_off();
        s_is_dimmed = false;
    } else if (!is_off && !s_is_dimmed && inactivity_ms > 30000) {
        ESP_LOGI(TAG, "Inactividad > 30s. Atenuando pantalla al 10%%.");
        read_user_brightness_from_nvs();
        screen_manager_set_brightness(10);
        s_is_dimmed = true;
    }
}

static void setup_inactivity_handling(void) {
    lv_timer_create(inactivity_timer_cb, 5000, NULL);
    lv_obj_t * scr = lv_screen_active();
    if (scr) {
        // Usamos LV_EVENT_ALL para capturar tanto PRESS como CLICK
        lv_obj_add_event_cb(scr, screen_touch_event_cb, LV_EVENT_ALL, NULL);
    }
    ESP_LOGI(TAG, "Gestor de inactividad y despertar configurado.");
}

// --- Flujo Principal de la Aplicación ---
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Sistema NVS inicializado.");

    ui_preinit();

    if (check_file_server_mode_flag()) {
        erase_file_server_mode_flag();
        run_file_server_mode();
    } else if (!wifi_portal_credentials_saved()) {
        run_wifi_portal_mode();
    } else {
        run_main_application_mode();
    }
}

static void init_lvgl_for_service_screen(void)
{
    ESP_LOGI(TAG, "Inicializando LVGL para pantalla de servicio...");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = bsp_get_panel_io_handle(),
        .panel_handle = bsp_get_display_handle(),
        .buffer_size = bsp_get_display_buffer_size(),
        .double_buffer = 1,
        .hres = bsp_get_display_hres(),
        .vres = bsp_get_display_vres(),
        .flags = {
            .swap_bytes = true,
        }
    };
    lv_disp_t * disp = lvgl_port_add_disp(&disp_cfg);

    ESP_LOGI(TAG, "Añadiendo driver de touch a LVGL para modos de servicio...");
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = bsp_get_touch_handle(),
    };
    lvgl_port_add_touch(&touch_cfg);
    
    setup_inactivity_handling();
}

static void run_file_server_mode(void) {
    ESP_LOGI(TAG, "Arrancando en modo Servidor de Archivos...");
    
    bsp_init_service_mode();
    init_lvgl_for_service_screen();
    
    if (lvgl_port_lock(0)) {
        ui_config_screen_show();
        lvgl_port_unlock();
    }

    bsp_wifi_init_stack();
    bsp_wifi_init_sta_from_nvs();
    bool ip_ok = bsp_wifi_wait_for_ip(15000);

    char ip_addr_buffer[16] = "N/A";
    if (ip_ok) {
        bsp_wifi_get_ip(ip_addr_buffer);
        ESP_LOGI(TAG, "Dispositivo conectado. IP: %s. Iniciando servidor web.", ip_addr_buffer);
    } else {
        ESP_LOGW(TAG, "No se pudo conectar. Iniciando en modo Punto de Acceso (AP).");
        bsp_wifi_start_ap();
        ESP_LOGI(TAG, "Punto de Acceso iniciado. Conéctate a 'DIYTogether' (pass: MakeItYours) y navega a http://192.168.4.1");
    }

    display_network_status_on_screen(ip_ok, ip_addr_buffer);
    
    ESP_LOGI(TAG, "Iniciando servidor web...");
    web_server_start();
}

static void run_wifi_portal_mode(void) {
    ESP_LOGI(TAG, "No hay credenciales. Arrancando en modo Portal WiFi...");
    
    bsp_init_service_mode();
    init_lvgl_for_service_screen();
    
    if (lvgl_port_lock(0)) {
        ui_config_screen_show();
        lvgl_port_unlock();
    }
    
    display_network_status_on_screen(false, NULL);

    bsp_wifi_init_stack();
    wifi_portal_start();
}

static void run_main_application_mode(void) {
    ESP_LOGI(TAG, "Cargando aplicación principal...");
    hardware_manager_init();
    screen_manager_init();
    
    ESP_LOGI(TAG, "El driver WiFi permanece desactivado para ahorrar RAM.");
    
    diymon_evolution_init();
    
    ui_assets_init();

    if (lvgl_port_lock(0)) {
        ui_init();
        setup_inactivity_handling();
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Interfaz de Usuario principal inicializada.");
    
    ESP_LOGI(TAG, "¡Firmware DIYMON en marcha!");
}

static bool check_file_server_mode_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) return false;

    char flag_val[8] = {0};
    size_t len = sizeof(flag_val);
    err = nvs_get_str(nvs_handle, "file_server", flag_val, &len);
    nvs_close(nvs_handle);
    return (err == ESP_OK && strcmp(flag_val, "1") == 0);
}

static void erase_file_server_mode_flag(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_erase_key(nvs_handle, "file_server");
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

static void display_network_status_on_screen(bool is_connected, const char* ip_addr) {
    if (lvgl_port_lock(0)) {
        lv_obj_t *scr = lv_screen_active();
        if (!scr) {
            lvgl_port_unlock();
            return;
        }

        static lv_style_t style_text;
        lv_style_init(&style_text);
        lv_style_set_text_color(&style_text, lv_color_white());
        lv_style_set_text_align(&style_text, LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_font(&style_text, &lv_font_montserrat_14);

        lv_obj_t *line1 = lv_label_create(scr);
        lv_obj_t *line2 = lv_label_create(scr);
        lv_obj_add_style(line1, &style_text, 0);
        lv_obj_add_style(line2, &style_text, 0);
        lv_obj_set_width(line1, 160);
        lv_obj_set_width(line2, 160);

        if (is_connected) {
            char ssid[33] = "N/A";
            size_t len = sizeof(ssid);
            nvs_handle_t nvs;
            if (nvs_open("storage", NVS_READONLY, &nvs) == ESP_OK) {
                nvs_get_str(nvs, "wifi_ssid", ssid, &len);
                nvs_close(nvs);
            }
            lv_label_set_text_fmt(line1, "Conectado a:\n\"%s\"", ssid);
            lv_label_set_text_fmt(line2, "IP: %s", ip_addr);
            lv_obj_align(line1, LV_ALIGN_CENTER, 0, -50);
            lv_obj_align(line2, LV_ALIGN_CENTER, 0, -20);
        } else {
            lv_label_set_text(line1, "Modo AP Activo");
            lv_label_set_text(line2, "SSID: DIYTogether\nPass: MakeItYours");
            lv_obj_align(line1, LV_ALIGN_CENTER, 0, -50);
            lv_obj_align(line2, LV_ALIGN_CENTER, 0, -25);
        }

        lvgl_port_unlock();
    }
}
