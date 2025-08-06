/*
 * Archivo: components/diymon_bsp/WS1.9TS/bsp_i2c.c
 * Versión Corregida: Implementación del driver I2C.
 */
#include "bsp_api.h" // ¡Incluir siempre la API pública primero!
#include "esp_log.h"
// No es necesario incluir bsp_i2c.h si no tiene contenido útil.

// --- CONFIGURACIÓN PRIVADA PARA ESTE MÓDULO ---
static const char *TAG = "bsp_i2c";

// Pines I2C correctos para la placa Waveshare
#define PIN_I2C_SCL      (GPIO_NUM_8)
#define PIN_I2C_SDA      (GPIO_NUM_18)
#define I2C_PORT_NUM     (I2C_NUM_0)

// Handle del bus I2C. Es estático (privado) para este archivo.
// Nadie más necesita acceder a él directamente.
static i2c_master_bus_handle_t g_bus_handle = NULL;

// --- IMPLEMENTACIÓN DE LAS FUNCIONES PÚBLICAS ---

// Esta es la implementación de la función prometida en bsp_api.h
esp_err_t bsp_i2c_init(void)
{
    ESP_LOGI(TAG, "Initializing I2C master bus on SCL:%d, SDA:%d", PIN_I2C_SCL, PIN_I2C_SDA);

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_NUM,
        .scl_io_num = PIN_I2C_SCL,
        .sda_io_num = PIN_I2C_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    // La función que crea el bus ahora guarda el handle en nuestra variable estática g_bus_handle
    return i2c_new_master_bus(&i2c_mst_config, &g_bus_handle);
}

// Esta función simplemente devuelve el handle que ya hemos inicializado y guardado.
i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void)
{
    return g_bus_handle;
}