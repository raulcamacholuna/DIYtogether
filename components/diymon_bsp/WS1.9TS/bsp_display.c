/*
Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_bsp\WS1.9TS\bsp_display.c
Fecha: $timestamp
Último cambio: Modificada la lógica de `bsp_display_set_brightness` para no guardar el estado de atenuación (10%) en NVS.
Descripción: Driver del display. Se ha modificado la función de ajuste de brillo para que los cambios automáticos de atenuación (al 10%) no sobrescriban el brillo configurado por el usuario en la NVS ni el último nivel de brillo conocido. Esto asegura que al reactivar la pantalla, esta vuelva al estado deseado por el usuario y no al 10%.
*/
#include "bsp_api.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "bsp_display";

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

static esp_lcd_panel_handle_t g_panel_handle = NULL;
static esp_lcd_panel_io_handle_t g_io_handle = NULL;

// Variable estática para guardar el último nivel de brillo
static int s_last_brightness_percentage = 100;

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

    // Carga el último nivel de brillo guardado en NVS.
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    int32_t saved_brightness = 100; // Valor por defecto si no se encuentra nada.
    if (err == ESP_OK) {
        err = nvs_get_i32(nvs_handle, "brightness", &saved_brightness);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "No se encontró brillo en NVS, usando valor por defecto (100).");
            saved_brightness = 100;
        } else {
            ESP_LOGI(TAG, "Brillo cargado desde NVS: %d%%", (int)saved_brightness);
        }
        nvs_close(nvs_handle);
    } else {
        ESP_LOGE(TAG, "Error al abrir NVS para leer brillo: %s", esp_err_to_name(err));
    }
    bsp_display_set_brightness((int)saved_brightness);

    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_LCD_CS, .dc_gpio_num = PIN_NUM_LCD_DC,
        .spi_mode = 0, .pclk_hz = 40 * 1000 * 1000, .trans_queue_depth = 10,
        .lcd_cmd_bits = 8, .lcd_param_bits = 8
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_SPI_HOST, &io_config, &g_io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST, .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(g_io_handle, &panel_config, &g_panel_handle));
    
    esp_lcd_panel_reset(g_panel_handle);
    esp_lcd_panel_init(g_panel_handle);
    
    esp_lcd_panel_swap_xy(g_panel_handle, true);
    esp_lcd_panel_mirror(g_panel_handle, false, true);
    esp_lcd_panel_set_gap(g_panel_handle, 35, 0); 
    esp_lcd_panel_invert_color(g_panel_handle, true);

    esp_lcd_panel_disp_on_off(g_panel_handle, true);
    
    ESP_LOGI(TAG, "Display initialized and rotated by driver to 170x320.");
    return ESP_OK;
}

void bsp_display_set_brightness(int percentage) {
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;

    // Calcula y establece el ciclo de trabajo del PWM para el backlight.
    uint32_t duty = 255 - ((255 * percentage) / 100);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    // El valor 10% es el nivel de atenuación automática por inactividad.
    // No se debe guardar en NVS ni como último brillo conocido del usuario.
    if (percentage == 10) {
        ESP_LOGD(TAG, "Atenuando pantalla temporalmente a 10%%. No se guarda el estado.");
        return;
    }

    // Guarda el último nivel de brillo (si es > 0) para la función de re-encendido.
    if (percentage > 0) {
        s_last_brightness_percentage = percentage;
    }

    // Guarda el nuevo nivel de brillo en la memoria no volátil (NVS).
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_set_i32(nvs_handle, "brightness", (int32_t)percentage);
        if (err == ESP_OK) {
            nvs_commit(nvs_handle);
            ESP_LOGD(TAG, "Brillo de usuario (%d%%) guardado en NVS.", percentage);
        } else {
            ESP_LOGE(TAG, "Error al guardar brillo en NVS: %s", esp_err_to_name(err));
        }
        nvs_close(nvs_handle);
    } else {
        ESP_LOGE(TAG, "Error al abrir NVS para guardar brillo: %s", esp_err_to_name(err));
    }
}

void bsp_display_turn_on(void) {
    ESP_LOGI(TAG, "Turning display ON and restoring brightness to %d%%", s_last_brightness_percentage);
    // 1. Restaurar el brillo (sin guardar estado, ya que se restaura uno existente)
    uint32_t duty = 255 - ((255 * s_last_brightness_percentage) / 100);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    // 2. Encender el panel
    esp_lcd_panel_disp_on_off(g_panel_handle, true);
}

void bsp_display_turn_off(void) {
    ESP_LOGI(TAG, "Turning display and backlight OFF");
    // 1. Apagar el panel
    esp_lcd_panel_disp_on_off(g_panel_handle, false);
    // 2. Apagar el backlight directamente para no llamar a set_brightness
    uint32_t duty = 255; // Duty 255 es 0% de brillo en mi configuración invertida
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

esp_lcd_panel_handle_t bsp_get_display_handle(void) { return g_panel_handle; }
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void) { return g_io_handle; }
int bsp_get_display_hres(void) { return BSP_LCD_H_RES; }
int bsp_get_display_vres(void) { return BSP_LCD_V_RES; }
size_t bsp_get_display_buffer_size(void) { return BSP_LCD_H_RES * 20; }
