/* Fichero: components/bsp/bsp.c */
/* Último cambio: Creado como fichero unificado para la orquestación del BSP, eliminando la duplicación de código. */
/* Descripción: Orquestador principal del BSP. Este fichero es común para todas las placas soportadas. Llama a las funciones de inicialización de los periféricos individuales (ej: bsp_i2c_init, bsp_display_init), cuyas implementaciones específicas de hardware se seleccionan en tiempo de compilación mediante directivas de preprocesador en sus respectivos ficheros .c. */
/* Último cambio: 20/08/2025 - 04:59 */
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
    
    if (bsp_sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tarjeta SD en modo headless.");
    }

    return ESP_OK;
}
