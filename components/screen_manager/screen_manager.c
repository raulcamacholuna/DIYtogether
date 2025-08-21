/* Fichero: components/screen_manager/screen_manager.c */
/* Descripción: Este módulo ahora propaga el nuevo parámetro 'save_to_nvs' a la capa inferior del BSP. Esto permite a los módulos de UI de más alto nivel, como el 'state_manager', controlar no solo el nivel de brillo sino también su persistencia. */
/* Último cambio: 21/08/2025 - 19:23 */
#include "screen_manager.h"
#include "bsp_api.h"
#include "esp_log.h"

static const char *TAG = "SCREEN_MANAGER";
static bool g_is_screen_off = false;

esp_err_t screen_manager_init(void) {
    ESP_LOGI(TAG, "Gestor de pantalla inicializado.");
    return ESP_OK;
}

void screen_manager_turn_on(void) {
    bsp_display_turn_on();
    g_is_screen_off = false;
    ESP_LOGI(TAG, "Petición para encender pantalla ejecutada.");
}

void screen_manager_turn_off(void) {
    bsp_display_turn_off();
    g_is_screen_off = true;
    ESP_LOGI(TAG, "Petición para apagar pantalla ejecutada.");
}

void screen_manager_set_brightness(int percentage, bool save_to_nvs) {
    bsp_display_set_brightness(percentage, save_to_nvs);
}

bool screen_manager_is_off(void) {
    return g_is_screen_off;
}
