/* Fichero: components/bsp/bsp_sdcard.c */
/* Último cambio: Unificado el código de los drivers de la tarjeta SD de ambas placas en un único fichero, utilizando directivas de preprocesador. */
/* Descripción: Este fichero contiene la lógica para la inicialización de la tarjeta SD. El pin Chip Select (CS), que es la única diferencia entre las placas, se selecciona en tiempo de compilación mediante las macros CONFIG_DIYTOGETHER_BOARD_*, definidas por PlatformIO. La lógica de montaje del sistema de ficheros FAT es idéntica y se comparte. */
/* Último cambio: 20/08/2025 - 05:01 */
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"

static const char *TAG = "bsp_sdcard";

// --- Definiciones de hardware específicas de cada placa ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9"
    #define PIN_NUM_CS    (GPIO_NUM_20)
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47"
    #define PIN_NUM_CS    (GPIO_NUM_4)
#else
    #error "No se ha definido una placa soportada en la configuración del proyecto"
#endif

#define MOUNT_POINT   "/sdcard"

// --- Variables estáticas globales del módulo ---
static sdmmc_card_t *g_card = NULL;
static sdmmc_host_t g_host = SDSPI_HOST_DEFAULT();

// --- Implementación de la función pública ---
esp_err_t bsp_sdcard_init(void)
{
    ESP_LOGI(TAG, "Initializing SD card (CS Pin: %d)...", PIN_NUM_CS);
    esp_err_t ret;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 10,
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
