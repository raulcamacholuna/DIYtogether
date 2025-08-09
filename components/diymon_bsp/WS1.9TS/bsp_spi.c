/*
 * Fichero: ./components/diymon_bsp/WS1.9TS/bsp_spi.c
 * Fecha: 08/08/2025 - 21:50
 * Último cambio: Añadida la inclusión de 'bsp_display.h' para resolver dependencias.
 * Descripción: Driver para la inicialización del bus SPI. Se incluye la cabecera
 *              del display para poder usar sus macros de resolución al configurar
 *              el tamaño máximo de transferencia del bus.
 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "bsp_display.h" // <-- SOLUCIÓN AL ERROR DE 'BSP_LCD_H_RES'

static const char *TAG = "bsp_spi";

#define BSP_SPI_HOST            SPI2_HOST
#define PIN_NUM_SPI_SCLK        5
#define PIN_NUM_SPI_MOSI        4
#define PIN_NUM_SPI_MISO        19 

esp_err_t bsp_spi_init(void) {
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
        ESP_LOGI(TAG, "SPI bus initialized successfully.");
    }

    return ret;
}