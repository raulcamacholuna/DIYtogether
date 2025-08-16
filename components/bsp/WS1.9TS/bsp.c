/* Fecha: 16/08/2025 - 08:17  */
/* Fichero: components/diymon_bsp/WS1.9TS/bsp.c */
/* Último cambio: Modificado para manejar el fallo de inicialización de la tarjeta SD sin abortar. */
/* Descripción: Orquestador del BSP. Ahora trata el fallo de la tarjeta SD como un error no fatal, permitiendo que el dispositivo arranque sin la SD. Esto es crucial para los modos de configuración que no dependen de los assets de la tarjeta y para la robustez general del sistema. */

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
    
    // [CORRECCIÓN] Manejar fallo de SD sin abortar.
    if (bsp_sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tarjeta SD, pero se continúa el arranque. Las animaciones no funcionarán.");
    }
    
    ESP_ERROR_CHECK(bsp_battery_init());
    return ESP_OK;
}

// Inicialización para modos de servicio que necesitan mostrar una imagen
esp_err_t bsp_init_service_mode(void) {
    ESP_LOGI(TAG, "Inicializando hardware para modo de servicio con pantalla...");
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_spi_init());
    
    // [CORRECCIÓN] Manejar fallo de SD sin abortar.
    if (bsp_sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tarjeta SD en modo servicio. El servidor de ficheros no funcionará.");
    }

    ESP_ERROR_CHECK(bsp_display_init());
    ESP_ERROR_CHECK(bsp_touch_init());
    bsp_display_set_brightness(100);
    return ESP_OK;
}

// Inicialización mínima para modos de servicio que no usan pantalla
esp_err_t bsp_init_minimal_headless(void) {
    ESP_LOGI(TAG, "Inicializando hardware MÍNIMO para modo headless (SPI + SD)...");
    ESP_ERROR_CHECK(bsp_spi_init());
    
    // [CORRECCIÓN] Manejar fallo de SD sin abortar.
    if (bsp_sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tarjeta SD en modo headless.");
    }

    return ESP_OK;
}
