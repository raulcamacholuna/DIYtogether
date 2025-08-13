/*
 * Fichero: ./main/hardware_manager.c
 * Fecha: 13/08/2025 - 05:33 
 * Último cambio: Establecido el formato de color del display a RGB565.
 * Descripción: Orquestador de hardware. Se ha añadido la llamada lv_display_set_color_format para configurar explícitamente el display en modo RGB565. Esto es crucial para que LVGL v9 realice correctamente la mezcla de canales alfa de las imágenes RGB565A8.
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
            .swap_bytes = true, // Corregir el orden de bytes de color para RGB565
        }
    };
    lv_disp_t * disp = lvgl_port_add_disp(&disp_cfg);

    // Configura el formato de color del framebuffer para la correcta mezcla de alpha.
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    /*
     * Configuración de la orientación del panel táctil.
     * Para alinear el sistema de coordenadas del táctil con la rotación del display (configurada en el BSP),
     * es necesario aplicar la misma secuencia de transformaciones que el display:
     * 1. Intercambiar los ejes X e Y (swap_xy en 'true').
     * 2. No aplicar espejo en el nuevo eje X (mirror_x en 'false').
     * 3. Aplicar espejo en el nuevo eje Y (mirror_y en 'true').
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

    // El brillo ahora se establece automáticamente desde la NVS durante bsp_init().
    return ESP_OK;
}
