/*
 * Archivo: components/diymon_bsp/WS1.9TS/bsp_sdcard.c
 * Versión: Final (con max_files aumentado)
 */
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "bsp_sdcard.h" 

static const char *TAG = "bsp_sdcard";

// --- CONFIGURACIÓN PRIVADA ---
#define PIN_NUM_MISO  (GPIO_NUM_19)
#define PIN_NUM_MOSI  (GPIO_NUM_4)
#define PIN_NUM_CLK   (GPIO_NUM_5)
#define PIN_NUM_CS    (GPIO_NUM_20)
#define MOUNT_POINT   "/sdcard"

static sdmmc_card_t *g_card = NULL;
static sdmmc_host_t g_host = SDSPI_HOST_DEFAULT();

// --- IMPLEMENTACIÓN DE LA FUNCIÓN PÚBLICA ---
esp_err_t bsp_sdcard_init(void)
{
    ESP_LOGI(TAG, "Initializing SD card...");
    esp_err_t ret;

    // 1. Configurar el sistema de ficheros FAT que se va a montar
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        // [CAMBIO CLAVE] Aumentamos el límite para permitir la navegación por subdirectorios.
        .max_files = 10,  // <-- El único cambio está aquí. Antes era 5.
        .allocation_unit_size = 16 * 1024
    };

    g_host.slot = SPI2_HOST;

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = SPI2_HOST;

    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &g_host, &slot_config, &mount_config, &g_card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s).", esp_err_to_name(ret));
        }
        return ret;
    }
    
    sdmmc_card_print_info(stdout, g_card);
    ESP_LOGI(TAG, "SD card initialized successfully!");
    
    return ESP_OK;
}