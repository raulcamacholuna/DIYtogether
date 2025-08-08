/*
 * Fichero: ./main/hardware_manager.c
 * Fecha: 08/08/2025 - 15:30
 * Último cambio: Corregida la orientación del panel táctil para alinear con el display.
 * Descripción: Orquestador de hardware. Inicializa y configura los periféricos, incluyendo el display y el panel táctil. Garantiza que la orientación del táctil se corresponda con la rotación del display configurada en el BSP.
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
    };
    lv_disp_t * disp = lvgl_port_add_disp(&disp_cfg);

    /*
     * Configuración de la orientación del panel táctil.
     * Para alinear el sistema de coordenadas del táctil con la rotación del display (configurada en el BSP),
     * es necesario aplicar la misma secuencia de transformaciones que el display:
     * 1. Intercambiar los ejes X e Y (`swap_xy` en 'true').
     * 2. No aplicar espejo en el nuevo eje X (`mirror_x` en 'false').
     * 3. Aplicar espejo en el nuevo eje Y (`mirror_y` en 'true').
     * Esta configuración asegura que un toque en una posición física de la pantalla se corresponda
     * con la misma posición en el framebuffer de LVGL.
     */
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

    bsp_display_set_brightness(100);
    return ESP_OK;
}