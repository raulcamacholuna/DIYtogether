/* Fecha: 24/08/2025 - 10:05
Fichero: components/screen_manager/screen_manager.c
Último cambio: Corrección de error de comentario y actualización de encabezado.
Descripción: */
#include "screen_manager.h"
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_lvgl_port.h"
#include <stdio.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SCREEN_MANAGER";
static bool g_is_screen_off = false;
static lv_obj_t *s_blackout_screen = NULL;

esp_err_t screen_manager_init(void) {
    if (lvgl_port_lock(0)) {
        s_blackout_screen = lv_obj_create(lv_layer_top());
        lv_obj_remove_style_all(s_blackout_screen);
        lv_obj_set_size(s_blackout_screen, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_color(s_blackout_screen, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(s_blackout_screen, LV_OPA_COVER, 0);
        lv_obj_add_flag(s_blackout_screen, LV_OBJ_FLAG_HIDDEN);
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Gestor de pantalla inicializado con blackout screen.");
    return ESP_OK;
}

void screen_manager_turn_on(void) {
    if (s_blackout_screen) {
        lv_obj_add_flag(s_blackout_screen, LV_OBJ_FLAG_HIDDEN);
    }
    bsp_display_turn_on();
    g_is_screen_off = false;
    ESP_LOGI(TAG, "Pantalla encendida.");
}

void screen_manager_turn_off(void) {
    if (g_is_screen_off) {
        return; 
    }

    if (s_blackout_screen) {
        ESP_LOGI(TAG, "Mostrando pantalla negra y forzando redibujado síncrono.");
        lv_obj_clear_flag(s_blackout_screen, LV_OBJ_FLAG_HIDDEN);
        lv_refr_now(NULL); // Fuerza el redibujado de inmediato.
    }

    bsp_display_turn_off();
    g_is_screen_off = true;
    ESP_LOGI(TAG, "Pantalla apagada (backlight off).");
}

void screen_manager_set_brightness(int percentage, bool save_to_nvs) {
    bsp_display_set_brightness(percentage, save_to_nvs);
}

bool screen_manager_is_off(void) {
    return g_is_screen_off;
}
