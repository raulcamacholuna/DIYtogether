/*
 * Archivo: bsp_display.c
 * Versión FINALÍSIMA: Contiene todos los includes, pines corregidos y la secuencia
 * de inicialización correcta para la pantalla ESP32-C6-LCD-1.9 de Waveshare.
 */

// --- PASO 1: LOS INCLUDES NECESARIOS (LA SOLUCIÓN AL ERROR) ---
#include "bsp_api.h"
#include "esp_log.h"
#include <stdbool.h> // Para poder usar 'true' y 'false'

// Headers para los drivers de hardware que usamos
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"


// --- PASO 2: DEFINICIONES PARA ESTE ARCHIVO ---
static const char *TAG = "bsp_display";

// Pines para la pantalla (CORREGIDOS según la wiki de Waveshare)
#define BSP_SPI_HOST            SPI2_HOST
#define PIN_NUM_LCD_SCLK        5
#define PIN_NUM_LCD_MOSI        4
#define PIN_NUM_LCD_CS          7
#define PIN_NUM_LCD_DC          6
#define PIN_NUM_LCD_RST         14
#define PIN_NUM_LCD_BL          15

// Pin MISO de la SD para la configuración del bus compartido
#define PIN_NUM_SD_MISO         19

// Configuración de la pantalla
#define BSP_LCD_H_RES           170
#define BSP_LCD_V_RES           320


// --- PASO 3: VARIABLES GLOBALES ESTÁTICAS ---
static esp_lcd_panel_handle_t g_panel_handle = NULL;
static esp_lcd_panel_io_handle_t g_io_handle = NULL;


// --- PASO 4: IMPLEMENTACIÓN DE LAS FUNCIONES ---

esp_err_t bsp_display_init(void) {
    ESP_LOGI(TAG, "Initializing display...");

    // 1. Inicializar el control del brillo (Backlight)
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

    // 2. Inicializar el bus SPI (configurado para la pantalla Y la SD)
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_LCD_SCLK, .mosi_io_num = PIN_NUM_LCD_MOSI,
        .miso_io_num = PIN_NUM_SD_MISO, .quadwp_io_num = -1, .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };
    ESP_ERROR_CHECK(spi_bus_initialize(BSP_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // 3. Crear el handle de I/O para la pantalla
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_LCD_CS, .dc_gpio_num = PIN_NUM_LCD_DC,
        .spi_mode = 0, .pclk_hz = 20 * 1000 * 1000, // 20MHz es una velocidad segura
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8, .lcd_param_bits = 8
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_SPI_HOST, &io_config, &g_io_handle));

    // 4. Crear el driver del panel LCD (para el controlador ST7789)
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST, .bits_per_pixel = 16
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(g_io_handle, &panel_config, &g_panel_handle));
    
    // 5. Secuencia de configuración de la pantalla
    ESP_LOGI(TAG, "Resetting and configuring display panel...");
    esp_lcd_panel_reset(g_panel_handle);
    esp_lcd_panel_init(g_panel_handle);
    esp_lcd_panel_swap_xy(g_panel_handle, true); 
    esp_lcd_panel_set_gap(g_panel_handle, 0, 0); 
    esp_lcd_panel_mirror(g_panel_handle, false, true);
    esp_lcd_panel_invert_color(g_panel_handle, false); 
    esp_lcd_panel_disp_on_off(g_panel_handle, true);

    ESP_LOGI(TAG, "Display initialized successfully!");
    return ESP_OK;
}

void bsp_display_set_brightness(int percentage) {
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;
    uint32_t duty = (255 * percentage) / 100;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

// --- Funciones "Getter" para que otros módulos accedan a los datos ---

esp_lcd_panel_handle_t bsp_get_display_handle(void) {
    return g_panel_handle;
}

esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void) {
    return g_io_handle;
}

int bsp_get_display_hres(void) {
    return BSP_LCD_H_RES;
}

int bsp_get_display_vres(void) {
    return BSP_LCD_V_RES;
}

size_t bsp_get_display_buffer_size(void) {
    return BSP_LCD_H_RES * 20;
}