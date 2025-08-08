/*
 * Archivo: bsp_touch.c
 * Versión: Corregida para la API I2C Legacy
 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/i2c.h" // Usamos la cabecera del driver I2C legacy
#include "esp_lcd_touch_cst816s.h"

static const char *TAG = "bsp_touch";
static esp_lcd_touch_handle_t g_touch_handle = NULL;

esp_err_t bsp_touch_init(void) {
    ESP_LOGI(TAG, "Initializing touch controller CST816S (Legacy API)...");
    
    // Configuración del IO del táctil.
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    
    // [LA SOLUCIÓN] Creamos la configuración manualmente sin el campo conflictivo.
    esp_lcd_panel_io_i2c_config_t tp_io_config = {
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS,
        .control_phase_bytes = 1,
        .dc_bit_offset = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    
    // Pasamos el NÚMERO de puerto I2C, que ya fue inicializado en bsp_i2c.c
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_NUM_0, &tp_io_config, &tp_io_handle));
    
    // Configuración del driver táctil (sin cambios)
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = 170,
        .y_max = 320,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &g_touch_handle));
    
    ESP_LOGI(TAG, "Touch driver initialized successfully.");
    return ESP_OK;
}

esp_lcd_touch_handle_t bsp_get_touch_handle(void) {
    return g_touch_handle;
}