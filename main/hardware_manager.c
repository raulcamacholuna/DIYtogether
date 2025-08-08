#include "hardware_manager.h"
#include "esp_log.h"
#include "bsp_api.h" 
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h" // Necesario para el ST7789

static const char *TAG = "HW_MANAGER";

esp_err_t hardware_manager_init(void) {
    ESP_LOGI(TAG, "Initializing BSP...");
    bsp_init(); // Llama al orquestador del BSP

    ESP_LOGI(TAG, "Initializing LVGL port...");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    ESP_LOGI(TAG, "Registering display with LVGL...");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = 6, .cs_gpio_num = 7, .pclk_hz = 40 * 1000 * 1000,
        .lcd_cmd_bits = 8, .lcd_param_bits = 8, .spi_mode = 0, .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = 14, .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_disp_on_off(panel_handle, true);
    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, false, true);
    esp_lcd_panel_set_gap(panel_handle, 35, 0);
    esp_lcd_panel_invert_color(panel_handle, true);

    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle, .panel_handle = panel_handle,
        .hres = 170, .vres = 320, .double_buffer = 1, .buffer_size = 170 * 20,
    };
    lv_disp_t * disp = lvgl_port_add_disp(&disp_cfg);

    ESP_LOGI(TAG, "Registering touch with LVGL...");
    lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = bsp_get_touch_handle(),
    };
    lvgl_port_add_touch(&touch_cfg);

    bsp_display_set_brightness(100);
    return ESP_OK;
}