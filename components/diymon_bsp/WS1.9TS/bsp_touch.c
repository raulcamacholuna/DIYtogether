/*
  Fichero: ./components/diymon_bsp/WS1.9TS/bsp_touch.c
  Fecha: 12/08/2025 - 09:00
  Último cambio: Corregido para usar el bus I2C global en lugar de pasarlo.
  Descripción: Driver del panel táctil CST816S. Ahora utiliza la función pública
               `bsp_get_i2c_bus_handle` para obtener el manejador del bus I2C.
*/
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_lcd_touch_cst816s.h"

static const char *TAG = "bsp_touch";
static esp_lcd_touch_handle_t g_touch_handle = NULL;

esp_err_t bsp_touch_init(void)
{
    ESP_LOGI(TAG, "Initializing touch controller CST816S (Modern API)...");

    i2c_master_bus_handle_t bus_handle = bsp_get_i2c_bus_handle();
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus handle is not initialized!");
        return ESP_FAIL;
    }

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = {
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .scl_speed_hz = 400000,
        .flags = {
            .dc_low_on_data = 0,
            .disable_control_phase = 1,
        }
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(bus_handle, &tp_io_config, &tp_io_handle));

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = 170,
        .y_max = 320,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &g_touch_handle));
    
    ESP_LOGI(TAG, "Touch driver initialized successfully.");
    return ESP_OK;
}

esp_lcd_touch_handle_t bsp_get_touch_handle(void)
{
    return g_touch_handle;
}