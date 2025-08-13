/*
  Fichero: ./components/screen_manager/screen_manager.c
  Fecha: 13/08/2025 - 12:03 
  Último cambio: Eliminada completamente la lógica de 'shake-to-wake'.
  Descripción: Se ha eliminado toda la funcionalidad de shake-to-wake, incluyendo la tarea,
               el semáforo y la inicialización de la interrupción del BSP. Esta funcionalidad
               será re-implementada dentro de la nueva tarea de telemetría en la capa de la UI
               para consolidar la lógica y resolver conflictos de I2C.
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
