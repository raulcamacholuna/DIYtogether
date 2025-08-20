/* Fichero: components/bsp/bsp_spi.c */
/* Último cambio: Corregidos los pines SPI (SCLK/MOSI) para la placa de 1.47" para resolver el fallo de inicialización de la tarjeta SD. */
/* Descripción: Diagnóstico de Causa Raíz: El error 'sdmmc_card_init failed (0x107)' (ESP_ERR_TIMEOUT) es causado por una asignación incorrecta de los pines SPI para la placa de 1.47". El log muestra un fallo de comunicación fundamental, y la comparación con el esquemático oficial confirma que los pines SCLK y MOSI estaban intercambiados.
Solución Definitiva: Se han corregido las macros PIN_NUM_SPI_SCLK y PIN_NUM_SPI_MOSI dentro del bloque de preprocesador para la placa WS1.47TS para que coincidan con el hardware (SCLK=GPIO1, MOSI=GPIO2). Esto establece la conexión física correcta, permitiendo que el controlador SPI se comunique con la tarjeta SD y la inicialice con éxito. */
/* Último cambio: 20/08/2025 - 05:58 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/spi_master.h"

static const char *TAG = "bsp_spi";

// --- Definiciones de hardware específicas de cada placa ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9"
    #define PIN_NUM_SPI_SCLK        5
    #define PIN_NUM_SPI_MOSI        4
    #define PIN_NUM_SPI_MISO        19 
    #define BSP_LCD_H_RES_FOR_SPI   170 // Resolución para calcular el tamaño del buffer
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47"
    // [CORRECCIÓN] Pines intercambiados según el esquemático oficial.
    #define PIN_NUM_SPI_SCLK        1
    #define PIN_NUM_SPI_MOSI        2
    #define PIN_NUM_SPI_MISO        3
    #define BSP_LCD_H_RES_FOR_SPI   172 // Resolución para calcular el tamaño del buffer
#else
    #error "No se ha definido una placa soportada en la configuración del proyecto"
#endif

#define BSP_SPI_HOST            SPI2_HOST

// --- Variables estáticas globales del módulo ---
static bool g_spi_bus_initialized = false;

// --- Implementación de funciones ---
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
        .max_transfer_sz = BSP_LCD_H_RES_FOR_SPI * 100 * sizeof(uint16_t)
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
