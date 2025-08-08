/*
 * Fichero: ./diymon_bsp/WS1.9TS/bsp_i2c.c
 * Fecha: 08/08/2025 - 03:30
 * Último cambio: Reescrito para usar la API I2C moderna (`i2c_master`).
 * Descripción: Centraliza la inicialización del bus I2C utilizando la API recomendada `i2c_master`. Crea un manejador de bus (`bus_handle`) que será compartido por todos los dispositivos I2C (táctil, IMU).
 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static const char *TAG = "bsp_i2c";

// Pines definidos para el bus I2C
#define PIN_I2C_SCL      8
#define PIN_I2C_SDA      18
#define I2C_PORT_NUM     I2C_NUM_0
#define I2C_CLK_SPEED_HZ 400000

// Variable estática para almacenar el manejador del bus I2C
static i2c_master_bus_handle_t g_bus_handle = NULL;

esp_err_t bsp_i2c_init(void)
{
    ESP_LOGI(TAG, "Initializing I2C master bus (Modern API)...");

    i2c_master_bus_config_t i2c_bus_config = {
        .i2c_port = I2C_PORT_NUM,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &g_bus_handle));

    ESP_LOGI(TAG, "I2C bus initialized successfully.");
    return ESP_OK;
}

i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void)
{
    return g_bus_handle;
}