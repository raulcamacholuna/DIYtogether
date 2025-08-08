/*
 * Archivo: bsp_i2c.c
 * Versión: Corregida con la API I2C Legacy para compatibilidad
 */
#include "bsp_api.h"
#include "esp_log.h"
// [CORRECCIÓN] Usamos la cabecera del driver I2C legacy
#include "driver/i2c.h"

static const char *TAG = "bsp_i2c";

#define I2C_PORT_NUM     I2C_NUM_0
#define PIN_I2C_SCL      8
#define PIN_I2C_SDA      18

// Esta es la implementación de la función prometida en bsp_api.h
esp_err_t bsp_i2c_init(void)
{
    ESP_LOGI(TAG, "Initializing I2C master bus (LEGACY API)...");

    // Usamos la estructura y funciones de la API antigua
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000, // 400KHz
    };
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT_NUM, conf.mode, 0, 0, 0));
    
    ESP_LOGI(TAG, "I2C bus initialized successfully.");
    return ESP_OK;
}

// Ya no necesitamos la función bsp_get_i2c_bus_handle()