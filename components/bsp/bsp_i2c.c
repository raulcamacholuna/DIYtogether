/* Fichero: components/bsp/bsp_i2c.c */
/* Último cambio: Unificado el código de los drivers I2C de ambas placas en un único fichero, utilizando directivas de preprocesador. */
/* Descripción: Este fichero contiene la lógica para la inicialización del bus I2C. Los pines SCL y SDA, que son la única diferencia entre las placas, se seleccionan en tiempo de compilación mediante las macros CONFIG_DIYTOGETHER_BOARD_*, definidas por PlatformIO. La lógica de inicialización idempotente es común y se mantiene sin cambios. */
/* Último cambio: 20/08/2025 - 05:00 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static const char *TAG = "bsp_i2c";

// --- Definiciones de hardware específicas de cada placa ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9"
    #define PIN_I2C_SCL      8
    #define PIN_I2C_SDA      18
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47"
    #define PIN_I2C_SCL      19
    #define PIN_I2C_SDA      18
#else
    #error "No se ha definido una placa soportada en la configuración del proyecto"
#endif

#define I2C_PORT_NUM     I2C_NUM_0
#define I2C_CLK_SPEED_HZ 400000

// --- Variables estáticas globales del módulo ---
static i2c_master_bus_handle_t g_bus_handle = NULL;
static bool g_i2c_bus_initialized = false;

// --- Implementación de funciones ---

esp_err_t bsp_i2c_init(void)
{
    if (g_i2c_bus_initialized) {
        ESP_LOGD(TAG, "I2C master bus ya está inicializado.");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing I2C master bus (SCL: %d, SDA: %d)...", PIN_I2C_SCL, PIN_I2C_SDA);

    i2c_master_bus_config_t i2c_bus_config = {
        .i2c_port = I2C_PORT_NUM,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &g_bus_handle));

    g_i2c_bus_initialized = true;
    ESP_LOGI(TAG, "I2C bus initialized successfully.");
    return ESP_OK;
}

i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void)
{
    return g_bus_handle;
}
