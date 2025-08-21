/* Fichero: components/screen_manager/screen_manager.c */
/* Descripción: Diagnóstico: Waking from light sleep es un evento de hardware (GPIO interrupt) que se dispara con un solo toque, eludiendo la lógica de software 'double-double-click'. Causa Raíz: La función 'screen_manager_turn_off' encendía la pantalla inmediatamente después de despertar, impidiendo que el 'state_manager' tuviera la oportunidad de validar el patrón de gestos. Solución Definitiva: Se ha modificado 'screen_manager_turn_off' para que, después de despertar del Light Sleep, simplemente deshabilite el wakeup source y retorne, manteniendo la pantalla apagada. Esto devuelve el control al 'state_manager', cuyo event handler de toques ('screen_touch_event_cb') ya contiene la lógica de 'double-double-click'. El primer toque (que despierta el dispositivo) ahora es procesado por este handler como el inicio de la secuencia de desbloqueo, restaurando el comportamiento deseado. */
/* Último cambio: 21/08/2025 - 21:08 */
#include "screen_manager.h"
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/gpio.h"

static const char *TAG = "SCREEN_MANAGER";
static bool g_is_screen_off = false;

// --- Definiciones de hardware específicas de cada placa ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9"
    #define TOUCH_INT_PIN GPIO_NUM_NC
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47"
    #define TOUCH_INT_PIN 21
#else
    #define TOUCH_INT_PIN GPIO_NUM_NC
#endif

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

#if (TOUCH_INT_PIN != GPIO_NUM_NC)
    ESP_LOGI(TAG, "Configurando GPIO %d como fuente de despertar para Light Sleep...", TOUCH_INT_PIN);
    ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());
    ESP_ERROR_CHECK(gpio_wakeup_enable(TOUCH_INT_PIN, GPIO_INTR_LOW_LEVEL));
    
    ESP_LOGI(TAG, "Entrando en Light Sleep. Tocar la pantalla para despertar...");
    
    esp_light_sleep_start();
    
    ESP_LOGI(TAG, "Despertado de Light Sleep por interacción.");
    
    gpio_wakeup_disable(TOUCH_INT_PIN);
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_GPIO);
    
    // NO encender la pantalla aquí. Dejar que el state_manager decida si el gesto fue intencional.
#else
    ESP_LOGW(TAG, "No se ha definido un pin de interrupción táctil, no se entrará en Light Sleep.");
#endif
}

void screen_manager_set_brightness(int percentage, bool save_to_nvs) {
    bsp_display_set_brightness(percentage, save_to_nvs);
}

bool screen_manager_is_off(void) {
    return g_is_screen_off;
}
