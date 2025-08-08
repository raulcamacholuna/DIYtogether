/*
 * Archivo: bsp_display.c
 * Versión: VICTORIA FINAL (Completo y sin ambigüedades)
 * Descripción: Configuración definitiva para la pantalla ESP32-C6-LCD-1.9,
 *              traduciendo la configuración de la demo de Arduino que funciona.
 */
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

static const char *TAG = "bsp_display_final";

// --- Configuración de Hardware ---
#define BSP_SPI_HOST            SPI2_HOST
#define PIN_NUM_LCD_SCLK        5
#define PIN_NUM_LCD_MOSI        4
#define PIN_NUM_LCD_CS          7
#define PIN_NUM_LCD_DC          6
#define PIN_NUM_LCD_RST         14
#define PIN_NUM_LCD_BL          15
#define PIN_NUM_SD_MISO         19
#define BSP_LCD_H_RES           170
#define BSP_LCD_V_RES           320

// --- Variables Globales ---
static esp_lcd_panel_handle_t g_panel_handle = NULL;
static esp_lcd_panel_io_handle_t g_io_handle = NULL;

// --- Implementación de la Inicialización ---
esp_err_t bsp_display_init(void) {
    ESP_LOGI(TAG, "Initializing display with DEFINITIVE Arduino Demo settings...");

    // 1. Configuración del Backlight (Lógica Invertida para ACTIVO-BAJO)
    ledc_timer_config_t bl_timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&bl_timer_conf));

    ledc_channel_config_t bl_channel_conf = {
        .gpio_num = PIN_NUM_LCD_BL,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0, // Duty 0 = 100% Brillo en activo-bajo
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&bl_channel_conf));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    // 2. Configuración del Bus SPI (Preparado para ser compartido con la SD)
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_LCD_SCLK,
        .mosi_io_num = PIN_NUM_LCD_MOSI,
        .miso_io_num = PIN_NUM_SD_MISO,
        .max_transfer_sz = BSP_LCD_H_RES * 100 * sizeof(uint16_t)
    };
    // NOTA: Usamos spi_bus_initialize en lugar de spi_bus_add_device para que pueda ser compartido
    ESP_ERROR_CHECK(spi_bus_initialize(BSP_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // 3. Configuración del Panel I/O
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .spi_mode = 0,
        .pclk_hz = 40 * 1000 * 1000,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_SPI_HOST, &io_config, &g_io_handle));

    // 4. Configuración del Driver ST7789
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(g_io_handle, &panel_config, &g_panel_handle));
    
    // 5. Reset e Inicialización
    esp_lcd_panel_reset(g_panel_handle);
    esp_lcd_panel_init(g_panel_handle);

    // 6. Geometría y Color (Traducción 1:1 de la demo de Arduino)
    esp_lcd_panel_swap_xy(g_panel_handle, false);
    esp_lcd_panel_mirror(g_panel_handle, false, false);
    esp_lcd_panel_set_gap(g_panel_handle, 35, 0);
    esp_lcd_panel_invert_color(g_panel_handle, false); // Soluciona los colores psicodélicos

    // 7. Encender la pantalla
    esp_lcd_panel_disp_on_off(g_panel_handle, true);
    
    ESP_LOGI(TAG, "Display initialized successfully!");
    return ESP_OK;
}

// Lógica de brillo invertida
void bsp_display_set_brightness(int percentage) {
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;
    uint32_t duty = 255 - ((255 * percentage) / 100);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

// Funciones "Getter"
esp_lcd_panel_handle_t bsp_get_display_handle(void) { return g_panel_handle; }
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void) { return g_io_handle; }
int bsp_get_display_hres(void) { return BSP_LCD_H_RES; }
int bsp_get_display_vres(void) { return BSP_LCD_V_RES; }
size_t bsp_get_display_buffer_size(void) { return BSP_LCD_H_RES * 20; }