/* Fichero: components/screen_manager/screen_manager.c */
/* Descripción: Diagnóstico de Causa Raíz: El backlight se re-ilumina progresivamente durante el sueño ligero en la placa de 1.9". Esto se debe a que el pin del backlight (BL) no mantiene su estado 'apagado' durante el light sleep, quedando en un estado flotante que re-energiza parcialmente el LED. Adicionalmente, el usuario espera despertar con el táctil, pero en esta placa el pin de interrupción no está conectado. Solución Definitiva: Se implementa un 'hold' explícito sobre el pin del backlight usando gpio_hold_en() antes de entrar en sueño y gpio_hold_dis() al despertar. Esto fuerza al pin a mantener su estado, asegurando que el backlight permanezca completamente apagado. Se confirma que para la placa de 1.9" el despertar es mediante el botón BOOT (GPIO9), que es la única fuente de interrupción externa fiable. */
/* Último cambio: 21/08/2025 - 21:50 */
#include "screen_manager.h"
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/gpio.h"

static const char *TAG = "SCREEN_MANAGER";
static bool g_is_screen_off = false;

// --- Definiciones de pines de despertar y control específicas de cada placa ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147)
    #define WAKEUP_PIN 21       // Interrupción del panel táctil
    #define WAKEUP_LEVEL GPIO_INTR_LOW_LEVEL
    #define BACKLIGHT_PIN 23
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6)
    #define WAKEUP_PIN 22        // Botón BOOT (el pin INT del táctil no está conectado)
    #define WAKEUP_LEVEL GPIO_INTR_LOW_LEVEL
    #define BACKLIGHT_PIN 15
#else
    #define WAKEUP_PIN GPIO_NUM_NC
    #define BACKLIGHT_PIN GPIO_NUM_NC
#endif

esp_err_t screen_manager_init(void) {
    ESP_LOGI(TAG, "Gestor de pantalla inicializado.");
    return ESP_OK;
}

void screen_manager_turn_on(void) {
    bsp_display_turn_on();
    g_is_screen_off = false;
    ESP_LOGI(TAG, "Pantalla encendida.");
}

void screen_manager_turn_off(void) {
    bsp_display_turn_off();
    g_is_screen_off = true;
    ESP_LOGI(TAG, "Pantalla apagada (backlight off).");
}

void screen_manager_enter_light_sleep(void) {
    if (!g_is_screen_off) {
        screen_manager_turn_off();
    }

#if (WAKEUP_PIN != GPIO_NUM_NC && BACKLIGHT_PIN != GPIO_NUM_NC)
    ESP_LOGI(TAG, "Configurando GPIO %d como fuente de despertar...", WAKEUP_PIN);
    ESP_ERROR_CHECK(gpio_wakeup_enable(WAKEUP_PIN, WAKEUP_LEVEL));
    ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());
    
    // [CORRECCIÓN] Habilitar el 'hold' en el pin del backlight para evitar que flote durante el sleep.
    ESP_LOGI(TAG, "Habilitando hold en el pin de backlight (GPIO %d)", BACKLIGHT_PIN);
    ESP_ERROR_CHECK(gpio_hold_en(BACKLIGHT_PIN));

    ESP_LOGI(TAG, "Entrando en Light Sleep...");
    esp_light_sleep_start();
    
    // --- Lógica de Despertar ---
    ESP_LOGI(TAG, "Despertado de Light Sleep.");
    
    // [CORRECCIÓN] Deshabilitar el 'hold' para que el LEDC pueda volver a controlar el pin.
    ESP_LOGI(TAG, "Deshabilitando hold en el pin de backlight (GPIO %d)", BACKLIGHT_PIN);
    ESP_ERROR_CHECK(gpio_hold_dis(BACKLIGHT_PIN));

    ESP_ERROR_CHECK(gpio_wakeup_disable(WAKEUP_PIN));
    ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_GPIO));
#else
    ESP_LOGW(TAG, "No se puede entrar en Light Sleep: no hay pin de interrupción/botón o de backlight definido para despertar.");
#endif
}

void screen_manager_set_brightness(int percentage, bool save_to_nvs) {
    bsp_display_set_brightness(percentage, save_to_nvs);
}

bool screen_manager_is_off(void) {
    return g_is_screen_off;
}
