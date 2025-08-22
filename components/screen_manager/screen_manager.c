/* Fichero: components/screen_manager/screen_manager.c */
/* Descripción: Diagnóstico: La pantalla mostraba una imagen fantasma al apagar el backlight. Causa Raíz: Existía una condición de carrera entre la invalidación de la UI (hacer visible la pantalla negra) y el apagado físico del backlight. El uso de un temporizador asíncrono no garantizaba que el ciclo de renderizado de LVGL se completara a tiempo. Solución Definitiva: Se ha implementado un apagado síncrono. La función 'screen_manager_turn_off' ahora, después de hacer visible el 'blackout_screen', invoca 'lv_refr_now(NULL)'. Esta función bloquea la ejecución y fuerza un redibujado inmediato de la pantalla. Una vez que retorna, se tiene la garantía de que el framebuffer contiene una imagen negra, y solo entonces se apaga el backlight. Esto elimina la condición de carrera y resuelve el problema de la imagen fantasma de forma determinista.
/* Último cambio: 22/08/2025 - 08:18
*/
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
