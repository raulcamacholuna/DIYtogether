/*
  Fichero: ./components/screen_manager/screen_manager.c
  Fecha: 12/08/2025 - 09:00
  Último cambio: Sin cambios funcionales, se mantiene para consistencia.
  Descripción: Gestor de estado de la pantalla. Se encarga de la lógica de
               encendido, apagado y control de brillo, delegando en el BSP.
*/
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

void screen_manager_set_brightness(int percentage) {
    bsp_display_set_brightness(percentage);
}

bool screen_manager_is_off(void) {
    return g_is_screen_off;
}