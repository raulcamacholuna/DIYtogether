/*
 * Fichero: ./components/diymon_bsp/WS1.9TS/bsp_spi.c
 * Fecha: 12/08/2025 - 05:05 pm
 * Último cambio: Implementada la inicialización idempotente.
 * Descripción: Driver para la inicialización del bus SPI. La función sp_spi_init es ahora idempotente, asegurando que el bus SPI solo se inicializa una vez, previniendo crashes por múltiples llamadas.
 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "bsp_display.h" 

static const char *TAG = "bsp_spi";

#define BSP_SPI_HOST            SPI2_HOST
#define PIN_NUM_SPI_SCLK        5
#define PIN_NUM_SPI_MOSI        4
#define PIN_NUM_SPI_MISO        19 

// Bandera para asegurar la inicialización única (idempotencia)
static bool g_spi_bus_initialized = false;

esp_err_t bsp_spi_init(void) {
    if (g_spi_bus_initialized) {
        ESP_LOGD(TAG, "SPI bus (Host: %d) ya está inicializado.", BSP_SPI_HOST);
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing main SPI bus (Host: %d)...", BSP_SPI_HOST);

    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_SPI_SCLK, 
        .mosi_io_num = PIN_NUM_SPI_MOSI,
        .miso_io_num = PIN_NUM_SPI_MISO, 
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = BSP_LCD_H_RES * 100 * sizeof(uint16_t)
    };
    
    esp_err_t ret = spi_bus_initialize(BSP_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus!");
    } else {
        g_spi_bus_initialized = true;
        ESP_LOGI(TAG, "SPI bus initialized successfully.");
    }

    return ret;
}
