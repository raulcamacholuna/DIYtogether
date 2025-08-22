/* Fichero: components/screen_manager/screen_manager.c */
/* Descripción: Diagnóstico: Pérdida de comunicación serie y fallo de WiFi en la placa de 1.47" tras despertar de light sleep. Causa Raíz: El hardware de la placa de 1.47" no restaura correctamente el estado de los dominios de alimentación 'TOP' (USB-JTAG, SPIs) y 'MODEM' (WiFi/BT) si se apagan durante el light sleep. Solución Definitiva: Se ha implementado un workaround programático y específico para la placa de 1.47". Justo antes de entrar en light sleep, se invoca a 'esp_sleep_pd_config()' para forzar explícitamente que los dominios TOP y MODEM permanezcan encendidos ('ESP_PD_OPTION_ON'). Esto garantiza que los periféricos críticos mantengan la alimentación y su estado, resolviendo el problema de conectividad al despertar.
/* Último cambio: 22/08/2025 - 07:42
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
    
    ESP_LOGI(TAG, "Habilitando hold en el pin de backlight (GPIO %d)", BACKLIGHT_PIN);
    ESP_ERROR_CHECK(gpio_hold_en(BACKLIGHT_PIN));
    
    ESP_LOGI(TAG, "Vaciando buffers de log y esperando a la UART...");
    fflush(stdout);
    fflush(stderr);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "Deteniendo el port de LVGL...");
    lvgl_port_stop();

#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147)
    ESP_LOGW(TAG, "WORKAROUND: Forzando dominios TOP y MODEM a ON para la placa 1.47");
    esp_sleep_pd_config(ESP_PD_DOMAIN_TOP, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_MODEM, ESP_PD_OPTION_ON);
#endif

    ESP_LOGI(TAG, "Entrando en Light Sleep...");
    esp_light_sleep_start();
    
    // --- Lógica de Despertar ---
    ESP_LOGI(TAG, "Despertado de Light Sleep.");
    
    ESP_LOGI(TAG, "Reanudando el port de LVGL...");
    lvgl_port_resume();

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
