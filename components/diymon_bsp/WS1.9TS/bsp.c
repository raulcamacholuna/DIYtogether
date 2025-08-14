/*
Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_bsp\WS1.9TS\bsp.c
Fecha: $timestamp
Último cambio: Añadida la inicialización del driver táctil a los modos de servicio.
Descripción: Orquestador del BSP. Se añade la llamada a `bsp_touch_init` en la secuencia de `bsp_init_service_mode` para asegurar que el panel táctil esté disponible para las pantallas de configuración, solucionando un crash por puntero nulo al registrar el dispositivo de entrada en LVGL.
*/
#include "bsp_api.h"
#include "esp_err.h"
#include "esp_log.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp";

// Inicialización completa para la aplicación principal
esp_err_t bsp_init(void) {
    ESP_LOGI(TAG, "Inicializando TODO el hardware para la aplicación principal...");
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_spi_init());
    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init());
    ESP_ERROR_CHECK(bsp_imu_init());
    ESP_ERROR_CHECK(bsp_sdcard_init());
    ESP_ERROR_CHECK(bsp_battery_init());
    return ESP_OK;
}

// Inicialización para modos de servicio que necesitan mostrar una imagen
esp_err_t bsp_init_service_mode(void) {
    ESP_LOGI(TAG, "Inicializando hardware para modo de servicio con pantalla...");
    ESP_ERROR_CHECK(bsp_i2c_init()); // I2C es necesario para el touch
    ESP_ERROR_CHECK(bsp_spi_init());
    ESP_ERROR_CHECK(bsp_sdcard_init());
    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init()); // <-- CORRECCIÓN: Faltaba esta línea
    bsp_display_set_brightness(100);
    return ESP_OK;
}

// Inicialización mínima para modos de servicio que no usan pantalla
esp_err_t bsp_init_minimal_headless(void) {
    ESP_LOGI(TAG, "Inicializando hardware MÍNIMO para modo headless (SPI + SD)...");
    ESP_ERROR_CHECK(bsp_spi_init());
    ESP_ERROR_CHECK(bsp_sdcard_init());
    return ESP_OK;
}
