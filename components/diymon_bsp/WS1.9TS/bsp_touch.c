/*
 * Archivo: components/diymon_bsp/WS1.9TS/bsp_touch.c
 * Versión Corregida: Implementación del driver del panel táctil.
 */
#include "bsp_api.h" // ¡Siempre incluye tu API pública primero!
#include "esp_log.h"
#include "esp_lcd_touch_axs5106.h" // El driver específico del chip táctil

// --- CONFIGURACIÓN PRIVADA PARA ESTE ARCHIVO ---
static const char *TAG = "bsp_touch";

// Pines del Táctil para esta placa específica
#define PIN_TP_RST      (GPIO_NUM_NC) // No conectado en esta placa
#define PIN_TP_INT      (GPIO_NUM_1)  // Pin de interrupción del táctil

// Dirección I2C del chip táctil
#define TOUCH_I2C_ADDRESS (0x15)

// Handle global estático para el táctil.
static esp_lcd_touch_handle_t g_touch_handle = NULL;

// --- IMPLEMENTACIÓN DE LAS FUNCIONES PÚBLICAS ---

esp_err_t bsp_touch_init(void)
{
    ESP_LOGI(TAG, "Initializing touch driver (AXS5106)...");

    i2c_master_bus_handle_t bus_handle = bsp_get_i2c_bus_handle();
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus has not been initialized! Cannot init touch.");
        return ESP_FAIL;
    }

    // El driver del táctil necesita un "device handle", no un "bus handle".
    // Primero añadimos el dispositivo al bus para obtener su propio handle.
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = TOUCH_I2C_ADDRESS,
        .scl_speed_hz = 400000, // 400KHz
    };
    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    // Configuración del panel táctil
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = bsp_get_display_hres(),
        .y_max = bsp_get_display_vres(),
        .rst_gpio_num = PIN_TP_RST,
        .int_gpio_num = PIN_TP_INT,
        .flags = {
            .swap_xy = 0,
            .mirror_x = 1,
            .mirror_y = 0,
        }
    };

    // Ahora sí, creamos la instancia del driver con el "device handle"
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_axs5106(dev_handle, &tp_cfg, &g_touch_handle));

    ESP_LOGI(TAG, "Touch driver initialized successfully.");
    return ESP_OK;
}

esp_lcd_touch_handle_t bsp_get_touch_handle(void)
{
    return g_touch_handle;
}