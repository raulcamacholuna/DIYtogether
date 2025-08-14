/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\main\hardware_manager.c
# Fecha: $timestamp
# Último cambio: Eliminado el driver de sistema de ficheros personalizado para LVGL.
# Descripción: Ahora que los iconos de los botones están compilados en el firmware, LVGL ya no necesita acceder a la tarjeta SD. Se ha eliminado todo el código del driver de sistema de ficheros personalizado, simplificando el arranque, reduciendo el tamaño del binario y eliminando código que ya era obsoleto.
*/
#include "hardware_manager.h"
#include "esp_log.h"
#include "bsp_api.h"
#include "esp_lvgl_port.h"

static const char *TAG = "HW_MANAGER";

esp_err_t hardware_manager_init(void) {
    ESP_LOGI(TAG, "Initializing BSP...");
    bsp_init(); 

    ESP_LOGI(TAG, "Initializing LVGL port...");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));
    
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = bsp_get_panel_io_handle(),
        .panel_handle = bsp_get_display_handle(),
        .buffer_size = bsp_get_display_buffer_size(),
        .double_buffer = 1,
        .hres = bsp_get_display_hres(),
        .vres = bsp_get_display_vres(),
        .flags = {
            .swap_bytes = true, 
        }
    };
    lv_disp_t * disp = lvgl_port_add_disp(&disp_cfg);

    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    ESP_LOGI(TAG, "Configuring touch driver orientation to match display rotation...");
    esp_lcd_touch_handle_t touch_handle = bsp_get_touch_handle();
    
    ESP_ERROR_CHECK(esp_lcd_touch_set_swap_xy(touch_handle, false));
    ESP_ERROR_CHECK(esp_lcd_touch_set_mirror_x(touch_handle, false));
    ESP_ERROR_CHECK(esp_lcd_touch_set_mirror_y(touch_handle, false));

    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = touch_handle,
    };
    lvgl_port_add_touch(&touch_cfg);
    
    return ESP_OK;
}
