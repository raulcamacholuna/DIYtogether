/*
Fichero: Z:\DIYTOGETHER\DIYtogether\components\service_screen\service_screen.c
Fecha: 14/08/2025 - 05:52 
Último cambio: Refactorizado para usar un asset de imagen compilado en el firmware en lugar de leer desde la SD.
Descripción: Lógica para mostrar una pantalla de servicio sin LVGL. Se ha modificado para dibujar directamente en el framebuffer del display un array de C que representa una imagen (el asset 'bg_config_img'), eliminando la dependencia de la tarjeta SD para los modos de servicio y haciéndolos más rápidos y robustos.
*/
#include "service_screen.h"
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_lcd_panel_ops.h" // Necesario para esp_lcd_panel_draw_bitmap
#include <lvgl.h> // Necesario para el tipo lv_img_dsc_t

static const char *TAG = "SERVICE_SCREEN";

// --- EXPLICACIÓN DEL PROCESO ---
// Para plasmar una imagen en pantalla sin LVGL, seguimos estos pasos:
// 1. Convertir la imagen (ej: "config_bg.png") a un fichero C usando el conversor de LVGL.
//    - Formato de color: RGB565 (para que coincida con el display).
//    - Salida: Array C.
// 2. Este proceso genera un fichero, por ejemplo "bg_config_img.c", que contiene
//    una estructura 'const lv_img_dsc_t bg_config_img = { ... };'.
// 3. Declaramos esa estructura como 'extern' para poder usarla aquí.
//    Esto le dice al compilador que la definición real está en otro fichero.
extern const lv_img_dsc_t bg_config;

// 4. Usamos la función de bajo nivel del driver del display sp_lcd_panel_draw_bitmap.
//    Esta función toma un puntero directo a los datos de píxeles (bg_config_img.data)
//    y los envía al hardware de la pantalla.

esp_err_t service_screen_show_from_rom(void) {
    ESP_LOGI(TAG, "Mostrando pantalla de servicio NATIVA desde asset en firmware...");
    
    // Asumimos que el hardware (SPI y display) ya ha sido inicializado por main.c
    
    // Obtenemos el manejador del display desde el BSP
    esp_lcd_panel_handle_t panel_handle = bsp_get_display_handle();
    if (!panel_handle) {
        ESP_LOGE(TAG, "El manejador del display no está inicializado.");
        return ESP_FAIL;
    }
    
    // Obtenemos las dimensiones para asegurarnos de que cubrimos toda la pantalla
    const int h_res = bsp_get_display_hres();
    const int v_res = bsp_get_display_vres();

    // Verificamos que las dimensiones de la imagen coincidan (opcional pero recomendado)
    if (bg_config.header.w != h_res || bg_config.header.h != v_res) {
        ESP_LOGW(TAG, "Las dimensiones de la imagen (%dx%d) no coinciden con las de la pantalla (%dx%d).",
                 bg_config.header.w, bg_config.header.h, h_res, v_res);
    }

    // Volcamos el búfer del asset directamente al display (sin LVGL)
    ESP_LOGI(TAG, "Renderizando imagen nativa en el display...");
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, h_res, v_res, bg_config.data);

    ESP_LOGI(TAG, "Pantalla de servicio mostrada con éxito.");
    return ESP_OK;
}

// La función original que cargaba desde la SD se mantiene por si se necesita,
// pero la nueva lógica usa la ROM.
esp_err_t service_screen_show_from_sd(const char* image_path) {
    ESP_LOGE(TAG, "service_screen_show_from_sd está obsoleta. Usar service_screen_show_from_rom.");
    return ESP_ERR_NOT_SUPPORTED;
}
