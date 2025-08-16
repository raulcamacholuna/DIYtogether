/* Fecha: 16/08/2025 - 08:24  */
/* Fichero: main/main.c */
/* Último cambio: Añadido el include de 'actions.h' para resolver el error de compilación. */
/* Descripción: Se ha añadido `#include "actions.h"` para que las funciones y enums definidos en ese fichero (como `execute_diymon_action` y `ACTION_ID_ACTIVATE_CONFIG_MODE`) sean visibles en `main.c`, corrigiendo el error de compilación por declaración implícita. */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "esp_lvgl_port.h"
#include "sys/stat.h"
#include <dirent.h>
#include "driver/gpio.h"

#include "bsp_api.h"
#include "hardware_manager.h"
#include "diymon_evolution.h"
#include "ui.h"
#include "wifi_portal.h"
#include "web_server.h"
#include "screen_manager.h"
#include "ui_config_screen.h"
#include "ui_asset_loader.h" 
#include "actions.h" // [CORRECCIÓN] Include faltante

#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "DIYMON_MAIN";

#define CONFIG_BUTTON_GPIO GPIO_NUM_9

// --- Declaraciones de funciones ---
static void run_file_server_mode(void);
static void run_wifi_portal_mode(void);
static void run_headless_wifi_portal_mode(void);
static void run_main_application_mode(void);
static bool check_file_server_mode_flag(void);
static void erase_file_server_mode_flag(void);
static void init_lvgl_for_service_screen(void);
static void display_network_status_on_screen(bool is_connected, const char* ip_addr);
static void display_waiting_for_network_on_screen(void);
static void setup_inactivity_handling(void);
static bool verify_sdcard_contents(void);

// --- Lógica de Gestión de Inactividad y Despertar ---
typedef enum { WAKE_STATE_OFF, WAKE_STATE_PRIMED, } wake_up_state_t;
static bool s_is_dimmed = false;
static int s_user_brightness = 100;
static bool s_user_brightness_known = false;
static wake_up_state_t s_wake_state = WAKE_STATE_OFF;
static uint8_t s_wake_click_count = 0;
static lv_timer_t *s_double_click_timer = NULL;
static lv_timer_t *s_wake_prime_timer = NULL;
static lv_obj_t *s_status_line1 = NULL;
static lv_obj_t *s_status_line2 = NULL;

static void read_user_brightness_from_nvs(void) {
    if (s_user_brightness_known) return;
    nvs_handle_t nvs_handle;
    if (nvs_open("storage", NVS_READONLY, &nvs_handle) == ESP_OK) {
        nvs_get_i32(nvs_handle, "brightness", (int32_t*)&s_user_brightness);
        nvs_close(nvs_handle);
        s_user_brightness_known = true;
    }
}

static void double_click_timer_cb(lv_timer_t * timer) { s_wake_click_count = 0; s_double_click_timer = NULL; }
static void wake_prime_timer_cb(lv_timer_t * timer) { s_wake_state = WAKE_STATE_OFF; s_wake_click_count = 0; s_wake_prime_timer = NULL; }

static void screen_touch_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;
    lv_display_trigger_activity(disp);
    if (code == LV_EVENT_PRESSED && s_is_dimmed && !screen_manager_is_off()) {
        read_user_brightness_from_nvs(); screen_manager_set_brightness(s_user_brightness); s_is_dimmed = false; return;
    }
    if (code == LV_EVENT_CLICKED && screen_manager_is_off()) {
        if (s_wake_state == WAKE_STATE_OFF) {
            s_wake_click_count++;
            if (s_wake_click_count == 1) { s_double_click_timer = lv_timer_create(double_click_timer_cb, 500, NULL); lv_timer_set_repeat_count(s_double_click_timer, 1); }
            else if (s_wake_click_count == 2) { if (s_double_click_timer) lv_timer_del(s_double_click_timer); s_double_click_timer = NULL; s_wake_state = WAKE_STATE_PRIMED; s_wake_click_count = 0; s_wake_prime_timer = lv_timer_create(wake_prime_timer_cb, 3000, NULL); lv_timer_set_repeat_count(s_wake_prime_timer, 1); }
        } else if (s_wake_state == WAKE_STATE_PRIMED) {
            s_wake_click_count++;
            if (s_wake_click_count == 1) { s_double_click_timer = lv_timer_create(double_click_timer_cb, 500, NULL); lv_timer_set_repeat_count(s_double_click_timer, 1); }
            else if (s_wake_click_count >= 2) { if (s_double_click_timer) lv_timer_del(s_double_click_timer); if (s_wake_prime_timer) lv_timer_del(s_wake_prime_timer); s_double_click_timer = NULL; s_wake_prime_timer = NULL; screen_manager_turn_on(); s_is_dimmed = false; s_wake_state = WAKE_STATE_OFF; s_wake_click_count = 0; }
        }
    }
}

static void inactivity_timer_cb(lv_timer_t * timer) {
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;
    uint32_t inactivity_ms = lv_display_get_inactive_time(disp);
    bool is_off = screen_manager_is_off();
    if (inactivity_ms < 30000 && s_is_dimmed) s_is_dimmed = false;
    if (!is_off && inactivity_ms > 60000) { screen_manager_turn_off(); s_is_dimmed = false; }
    else if (!is_off && !s_is_dimmed && inactivity_ms > 30000) { read_user_brightness_from_nvs(); screen_manager_set_brightness(10); s_is_dimmed = true; }
}

static void setup_inactivity_handling(void) {
    lv_timer_create(inactivity_timer_cb, 5000, NULL);
    lv_obj_t * scr = lv_screen_active();
    if (scr) {
        lv_obj_add_event_cb(scr, screen_touch_event_cb, LV_EVENT_ALL, NULL);
    }
}

void app_main(void) {
    nvs_flash_init();
    ui_preinit();

    gpio_config_t btn_conf = { .pin_bit_mask = (1ULL << CONFIG_BUTTON_GPIO), .mode = GPIO_MODE_INPUT, .pull_up_en = GPIO_PULLUP_ENABLE, };
    gpio_config(&btn_conf);
    vTaskDelay(pdMS_TO_TICKS(50)); 
    if (gpio_get_level(CONFIG_BUTTON_GPIO) == 0) {
        run_headless_wifi_portal_mode();
    }

    if (check_file_server_mode_flag()) { erase_file_server_mode_flag(); run_file_server_mode(); }
    else if (!wifi_portal_credentials_saved()) { run_wifi_portal_mode(); }
    else { run_main_application_mode(); }
    while (1) { vTaskDelay(pdMS_TO_TICKS(10000)); }
}

static bool verify_sdcard_contents(void) {
    const char* dir_path = "/sdcard/diymon";
    DIR* dir = opendir(dir_path);
    if (!dir) {
        ESP_LOGE(TAG, "¡ERROR! No se pudo abrir el directorio '%s'. Arrancando en modo de funcionalidad limitada.", dir_path);
        return false;
    }
    closedir(dir);
    ESP_LOGI(TAG, "Verificación de la SD completada.");
    return true;
}

static void run_main_application_mode(void) {
    ESP_LOGI(TAG, "Cargando aplicación principal...");
    hardware_manager_init();
    
    bool is_sd_ok = verify_sdcard_contents();

    if (is_sd_ok) {
        screen_manager_init();
        hardware_manager_mount_lvgl_filesystem();
        diymon_evolution_init();
    } else {
        diymon_evolution_init(); 
    }
    
    ui_assets_init();

    if (lvgl_port_lock(0)) {
        ui_init();
        setup_inactivity_handling();
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Interfaz de Usuario principal inicializada.");

    if (!is_sd_ok) {
        ESP_LOGW(TAG, "Fallo en la SD. Entrando automáticamente en modo de configuración WiFi...");
        vTaskDelay(pdMS_TO_TICKS(500));
        execute_diymon_action(ACTION_ID_ACTIVATE_CONFIG_MODE);
    } else {
        ESP_LOGI(TAG, "¡Firmware DIYMON en marcha!");
    }
}

static void run_headless_wifi_portal_mode(void) { bsp_init_minimal_headless(); bsp_wifi_init_stack(); wifi_portal_start(); }
static void run_file_server_mode(void) { bsp_init_service_mode(); init_lvgl_for_service_screen(); if (lvgl_port_lock(0)) { ui_config_screen_show(); setup_inactivity_handling(); lvgl_port_unlock(); } display_waiting_for_network_on_screen(); bsp_wifi_init_stack(); bsp_wifi_init_sta_from_nvs(); bool ip_ok = bsp_wifi_wait_for_ip(15000); char ip_addr_buffer[16] = "N/A"; if (ip_ok) { bsp_wifi_get_ip(ip_addr_buffer); } else { bsp_wifi_start_ap(); } display_network_status_on_screen(ip_ok, ip_addr_buffer); web_server_start(); }
static void run_wifi_portal_mode(void) { bsp_init_service_mode(); init_lvgl_for_service_screen(); if (lvgl_port_lock(0)) { ui_config_screen_show(); setup_inactivity_handling(); lvgl_port_unlock(); } display_waiting_for_network_on_screen(); vTaskDelay(pdMS_TO_TICKS(1500)); display_network_status_on_screen(false, NULL); bsp_wifi_init_stack(); wifi_portal_start(); }
static bool check_file_server_mode_flag(void) { nvs_handle_t nvs; esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs); if (err != ESP_OK) return false; char flag[8] = {0}; size_t len = sizeof(flag); err = nvs_get_str(nvs, "file_server", flag, &len); nvs_close(nvs); return (err == ESP_OK && strcmp(flag, "1") == 0); }
static void erase_file_server_mode_flag(void) { nvs_handle_t nvs; if (nvs_open("storage", NVS_READWRITE, &nvs) == ESP_OK) { nvs_erase_key(nvs, "file_server"); nvs_commit(nvs); nvs_close(nvs); } }
static void init_lvgl_for_service_screen(void) { const lvgl_port_cfg_t c = ESP_LVGL_PORT_INIT_CONFIG(); lvgl_port_init(&c); const lvgl_port_display_cfg_t d = { .io_handle=bsp_get_panel_io_handle(), .panel_handle=bsp_get_display_handle(), .buffer_size=bsp_get_display_buffer_size(), .double_buffer=1, .hres=bsp_get_display_hres(), .vres=bsp_get_display_vres(), .flags={.swap_bytes=true}}; lv_disp_t* disp=lvgl_port_add_disp(&d); const lvgl_port_touch_cfg_t t = {.disp=disp, .handle=bsp_get_touch_handle()}; lvgl_port_add_touch(&t); }
static void display_waiting_for_network_on_screen(void) { if(lvgl_port_lock(0)){lv_obj_t*s=lv_screen_active();if(s){if(!s_status_line1){static lv_style_t st;lv_style_init(&st);lv_style_set_text_color(&st,lv_color_white());lv_style_set_text_align(&st,LV_TEXT_ALIGN_CENTER);lv_style_set_text_font(&st,&lv_font_montserrat_14);s_status_line1=lv_label_create(s);s_status_line2=lv_label_create(s);lv_obj_add_style(s_status_line1,&st,0);lv_obj_add_style(s_status_line2,&st,0);lv_obj_set_width(s_status_line1,160);lv_obj_set_width(s_status_line2,160);}lv_label_set_text(s_status_line1,"Espera...");lv_label_set_text(s_status_line2,"Detectando redes");lv_obj_align(s_status_line1,LV_ALIGN_CENTER,0,-50);lv_obj_align(s_status_line2,LV_ALIGN_CENTER,0,-25);}lvgl_port_unlock();}}
static void display_network_status_on_screen(bool is_connected, const char* ip_addr) { if(lvgl_port_lock(0)){lv_obj_t*s=lv_screen_active();if(s && s_status_line1 && s_status_line2){if(is_connected){char ssid[33]="N/A";size_t len=sizeof(ssid);nvs_handle_t nvs;if(nvs_open("storage",NVS_READONLY,&nvs)==ESP_OK){nvs_get_str(nvs,"wifi_ssid",ssid,&len);nvs_close(nvs);}lv_label_set_text_fmt(s_status_line1,"Conectado a:\n\"%s\"",ssid);lv_label_set_text_fmt(s_status_line2,"IP: %s",ip_addr);lv_obj_align(s_status_line1,LV_ALIGN_CENTER,0,-50);lv_obj_align(s_status_line2,LV_ALIGN_CENTER,0,-20);}else{lv_label_set_text(s_status_line1,"Modo AP Activo");lv_label_set_text(s_status_line2,"SSID: DIYTogether\nPass: MakeItYours");lv_obj_align(s_status_line1,LV_ALIGN_CENTER,0,-50);lv_obj_align(s_status_line2,LV_ALIGN_CENTER,0,-25);}}lvgl_port_unlock();}}
