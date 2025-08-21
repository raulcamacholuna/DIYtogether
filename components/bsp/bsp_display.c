/* Fichero: components/bsp/bsp_display.c */
/* Descripción: Diagnóstico: La lógica de brillo actualizaba la NVS en cada llamada, impidiendo un atenuado temporal. Solución: Se refactoriza la función para aceptar un booleano 'save_to_nvs', desacoplando el cambio de brillo físico del guardado en memoria no volátil. Ahora solo se persiste el valor cuando es un cambio explícito del usuario. */
/* Último cambio: 21/08/2025 - 19:23 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "nvs_flash.h"
#include "nvs.h"

// --- Inclusión condicional de drivers de display ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6)
    #include "esp_lcd_panel_vendor.h" 
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147)
    #include "esp_lcd_jd9853.h" 
#endif

static const char *TAG = "bsp_display";

// --- Definiciones de hardware específicas de cada placa ---
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9"
    #define BSP_SPI_HOST            SPI2_HOST
    #define PIN_NUM_LCD_CS          7
    #define PIN_NUM_LCD_DC          6
    #define PIN_NUM_LCD_RST         14
    #define PIN_NUM_LCD_BL          15
    #define BSP_LCD_H_RES           170
    #define BSP_LCD_V_RES           320
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47"
    #define BSP_SPI_HOST            SPI2_HOST
    #define PIN_NUM_LCD_CS          14
    #define PIN_NUM_LCD_DC          15
    #define PIN_NUM_LCD_RST         22
    #define PIN_NUM_LCD_BL          23
    #define BSP_LCD_H_RES           172
    #define BSP_LCD_V_RES           320
#else
    #error "No se ha definido una placa soportada en la configuración del proyecto"
#endif

// --- Variables estáticas globales del módulo ---
static esp_lcd_panel_handle_t g_panel_handle = NULL;
static esp_lcd_panel_io_handle_t g_io_handle = NULL;
static int s_last_brightness_percentage = 100;

// --- Implementación de funciones ---
esp_err_t bsp_display_init(void) {
    ESP_LOGI(TAG, "Initializing display...");

    ledc_timer_config_t bl_timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE, .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0, .freq_hz = 5000, .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&bl_timer_conf));
    ledc_channel_config_t bl_channel_conf = {
        .gpio_num = PIN_NUM_LCD_BL, .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0, .timer_sel = LEDC_TIMER_0, .duty = 0, .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&bl_channel_conf));

    nvs_handle_t nvs_handle;
    int32_t saved_brightness = 100;
    if (nvs_open("storage", NVS_READONLY, &nvs_handle) == ESP_OK) {
        if (nvs_get_i32(nvs_handle, "brightness", &saved_brightness) != ESP_OK) saved_brightness = 100;
        nvs_close(nvs_handle);
    }
    bsp_display_set_brightness((int)saved_brightness, true);

    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_LCD_CS, .dc_gpio_num = PIN_NUM_LCD_DC,
        .spi_mode = 0, .pclk_hz = 40 * 1000 * 1000, .trans_queue_depth = 10,
        .lcd_cmd_bits = 8, .lcd_param_bits = 8
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_SPI_HOST, &io_config, &g_io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST, 
        .bits_per_pixel = 16,
    };

#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6)
    ESP_LOGI(TAG, "Configuring for ST7789 (1.9 inch board)");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(g_io_handle, &panel_config, &g_panel_handle));
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147)
    ESP_LOGI(TAG, "Configuring for JD9853 (1.47 inch board)");
    ESP_ERROR_CHECK(esp_lcd_new_panel_jd9853(g_io_handle, &panel_config, &g_panel_handle));
#endif

    esp_lcd_panel_reset(g_panel_handle);
    esp_lcd_panel_init(g_panel_handle);
    
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) // Placa 1.9"
    esp_lcd_panel_swap_xy(g_panel_handle, true);
    esp_lcd_panel_mirror(g_panel_handle, false, true);
    esp_lcd_panel_set_gap(g_panel_handle, 35, 0); 
    esp_lcd_panel_invert_color(g_panel_handle, true);
#elif defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147) // Placa 1.47"
    esp_lcd_panel_swap_xy(g_panel_handle, false);
    esp_lcd_panel_mirror(g_panel_handle, true, false); 
    esp_lcd_panel_set_gap(g_panel_handle, 34, 0);
    esp_lcd_panel_invert_color(g_panel_handle, true);
#endif

    esp_lcd_panel_disp_on_off(g_panel_handle, true);
    bsp_display_turn_on();
    ESP_LOGI(TAG, "Display initialized successfully.");
    return ESP_OK;
}

void bsp_display_set_brightness(int percentage, bool save_to_nvs) {
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;
    
    uint32_t duty = 255 - ((255 * percentage) / 100);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    if (save_to_nvs) {
        s_last_brightness_percentage = percentage;
        nvs_handle_t nvs_handle;
        if (nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
            nvs_set_i32(nvs_handle, "brightness", (int32_t)percentage);
            nvs_commit(nvs_handle);
            nvs_close(nvs_handle);
        }
    }
}

void bsp_display_turn_on(void) {
    uint32_t duty = 255 - ((255 * s_last_brightness_percentage) / 100);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    if (g_panel_handle) esp_lcd_panel_disp_on_off(g_panel_handle, true);
}

void bsp_display_turn_off(void) {
    if (g_panel_handle) esp_lcd_panel_disp_on_off(g_panel_handle, false);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 255);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

esp_lcd_panel_handle_t bsp_get_display_handle(void) { return g_panel_handle; }
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void) { return g_io_handle; }
int bsp_get_display_hres(void) { return BSP_LCD_H_RES; }
int bsp_get_display_vres(void) { return BSP_LCD_V_RES; }
size_t bsp_get_display_buffer_size(void) {
    return BSP_LCD_H_RES * 20 * sizeof(uint16_t);
}
