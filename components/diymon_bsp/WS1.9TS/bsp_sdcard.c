/*
 * Archivo: components/diymon_bsp/WS1.9TS/bsp_sdcard.c
 * Versión Corregida: Implementación del driver de la tarjeta SD con la llamada a la API correcta.
 */
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"

// Se asume que bsp_sdcard.h puede existir para defines privados,
// aunque para este ejemplo no es estrictamente necesario.
#include "bsp_sdcard.h" 

static const char *TAG = "bsp_sdcard";

// --- CONFIGURACIÓN PRIVADA ---
// Pines para la SD Card según el esquemático de la placa
#define PIN_NUM_MISO  (GPIO_NUM_19)
#define PIN_NUM_MOSI  (GPIO_NUM_4)
#define PIN_NUM_CLK   (GPIO_NUM_5)
#define PIN_NUM_CS    (GPIO_NUM_20)
#define MOUNT_POINT   "/sdcard"

// Handle estático para la tarjeta
static sdmmc_card_t *g_card = NULL;
// Handle para el bus SPI Host
static sdmmc_host_t g_host = SDSPI_HOST_DEFAULT();


// --- IMPLEMENTACIÓN DE LA FUNCIÓN PÚBLICA ---

esp_err_t bsp_sdcard_init(void)
{
    ESP_LOGI(TAG, "Initializing SD card...");
    esp_err_t ret;

    // 1. Configurar el sistema de ficheros FAT que se va a montar
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false, // No formatear si falla el montaje
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    // 2. Configurar el bus SPI específicamente para la SD Card
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    
    // Inicializamos el bus SPI2. El DMA se asigna automáticamente (SPI_DMA_CH_AUTO)
    // Es importante que si la pantalla ya inicializó este bus, no lo hagamos de nuevo.
    // Una mejora futura sería asegurar que el bus se inicializa una sola vez.
    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus (%s).", esp_err_to_name(ret));
        return ret;
    }
    // ESP_ERR_INVALID_STATE significa que el bus ya estaba inicializado, lo cual está bien.
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "SPI bus (SPI2_HOST) was already initialized.");
    }

    // 3. Configurar el HOST (el controlador del bus SPI para la SD)
    // La estructura g_host ya se inicializó con SDSPI_HOST_DEFAULT()
    g_host.slot = SPI2_HOST; // Le decimos al host qué bus SPI usar

    // 4. Configurar el SLOT (el dispositivo SD específico en el bus)
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = SPI2_HOST;

    // 5. Montar el sistema de ficheros con los argumentos en el ORDEN CORRECTO
    // esp_vfs_fat_sdspi_mount(mount_point, host_config, slot_config, mount_config, card_handle)
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &g_host, &slot_config, &mount_config, &g_card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors.", esp_err_to_name(ret));
        }
        return ret;
    }
    
    // Imprimir información de la tarjeta si todo fue bien
    sdmmc_card_print_info(stdout, g_card);
    ESP_LOGI(TAG, "SD card initialized successfully!");
    
    return ESP_OK;
}