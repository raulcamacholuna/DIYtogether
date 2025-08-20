/* Fichero: components/bsp/bsp_touch.c */
/* Último cambio: Corregida la inicialización del driver AXS5106 para usar la API de I2C correcta, resolviendo el crash 'Load access fault' y la advertencia de compilación. */
/* Descripción: Diagnóstico de Causa Raíz: La advertencia 'incompatible pointer type' y el crash subsecuente se deben a un error de API. El driver del chip táctil AXS5106 (placa 1.47") no utiliza la abstracción 'esp_lcd_panel_io_handle_t' como el CST816S (placa 1.9"), sino que requiere un manejador de dispositivo I2C de más bajo nivel ('i2c_master_dev_handle_t'). Se le estaba pasando un tipo de puntero incorrecto.
Solución Definitiva: Se ha refactorizado la rama de inicialización de la placa 1.47". En lugar de crear un 'panel_io', ahora se crea un dispositivo I2C directamente sobre el bus I2C maestro usando 'i2c_master_bus_add_device'. El manejador de dispositivo resultante ('dev_handle'), que es del tipo correcto, se pasa a 'esp_lcd_touch_new_i2c_axs5106'. Esto alinea el código con la API específica del driver, resuelve la advertencia de compilación y elimina el crash al garantizar que el dispositivo táctil se inicializa correctamente. */
/* Último cambio: 20/08/2025 - 06:39 */
#include "bsp_api.h"
#include "esp_log.h"

// --- Inclusión condicional de drivers ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6)
    #include "esp_lcd_touch_cst816s.h"
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147)
    #include "esp_lcd_touch_axs5106.h"
#else
    #error "Una placa debe ser definida. Ninguna macro de placa (ej: CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) está presente."
#endif

static const char *TAG = "bsp_touch";

// --- Definiciones de hardware específicas de cada placa ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9"
    #define PIN_NUM_TOUCH_RST GPIO_NUM_NC
    #define PIN_NUM_TOUCH_INT GPIO_NUM_NC
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47"
    #define PIN_NUM_TOUCH_RST 20
    #define PIN_NUM_TOUCH_INT 21
#endif

// --- Variables estáticas globales del módulo ---
static esp_lcd_touch_handle_t g_touch_handle = NULL;

// --- Implementación de funciones ---
esp_err_t bsp_touch_init(void)
{
    i2c_master_bus_handle_t bus_handle = bsp_get_i2c_bus_handle();
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus handle is not initialized!");
        return ESP_FAIL;
    }

// --- Lógica de selección de driver estricta ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47" (AXS5106)
    ESP_LOGI(TAG, "Initializing touch controller AXS5106 for 1.47 inch board...");
    
    // [CORRECCIÓN] El driver AXS5106 usa un i2c_master_dev_handle_t, no un panel_io_handle.
    i2c_master_dev_handle_t dev_handle;
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ESP_LCD_TOUCH_IO_I2C_AXS5106_ADDRESS,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = bsp_get_display_hres(), .y_max = bsp_get_display_vres(),
        .rst_gpio_num = PIN_NUM_TOUCH_RST, .int_gpio_num = PIN_NUM_TOUCH_INT,
        .levels = { .reset = 0, .interrupt = 0 }
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_axs5106(dev_handle, &tp_cfg, &g_touch_handle));

#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9" (CST816S)
    ESP_LOGI(TAG, "Initializing touch controller CST816S for 1.9 inch board...");
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = {
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS,
        .control_phase_bytes = 1, .lcd_cmd_bits = 8, .lcd_param_bits = 8, .scl_speed_hz = 400000,
        .flags = { .disable_control_phase = 1 }
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(bus_handle, &tp_io_config, &tp_io_handle));

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = bsp_get_display_hres(), .y_max = bsp_get_display_vres(),
        .rst_gpio_num = PIN_NUM_TOUCH_RST, .int_gpio_num = PIN_NUM_TOUCH_INT,
        .levels = { .reset = 0, .interrupt = 0 }
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &g_touch_handle));
#endif
    
    ESP_LOGI(TAG, "Touch driver initialized successfully.");
    return ESP_OK;
}

esp_lcd_touch_handle_t bsp_get_touch_handle(void)
{
    return g_touch_handle;
}
