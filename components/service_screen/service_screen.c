/*
  Fichero: ./components/service_screen/service_screen.c
  Fecha: 12/08/2025 - 10:00
  Último cambio: Reescrito para usar renderizado nativo sin LVGL.
  Descripción: Implementa la lógica para mostrar una pantalla de servicio estática.
               Ahora lee el fichero binario y lo vuelca directamente al framebuffer
               del display usando `esp_lcd_panel_draw_bitmap`, evitando por completo
               la inicialización de la tarea y los búferes de LVGL.
*/
#include "service_screen.h"
#include "bsp_api.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_ops.h" // Necesario para esp_lcd_panel_draw_bitmap
#include <stdlib.h>
#include <stdio.h>

static const char *TAG = "SERVICE_SCREEN";

esp_err_t service_screen_show(const char* image_path) {
    ESP_LOGI(TAG, "Mostrando pantalla de servicio NATIVA desde: %s", image_path);
    
    // 1. Inicializar solo el hardware de pantalla y SD
    bsp_init_service_mode();

    // 2. Obtener información y manejador del display
    esp_lcd_panel_handle_t panel_handle = bsp_get_display_handle();
    const int h_res = bsp_get_display_hres();
    const int v_res = bsp_get_display_vres();
    const size_t buf_size = h_res * v_res * 2; // 2 bytes por píxel (RGB565)

    // 3. Reservar memoria para la imagen
    uint8_t *img_buf = (uint8_t*)malloc(buf_size);
    if (!img_buf) {
        ESP_LOGE(TAG, "Fallo al reservar %d bytes para la imagen de servicio.", (int)buf_size);
        return ESP_ERR_NO_MEM;
    }

    // 4. Leer el fichero binario desde la SD
    FILE* f = fopen(image_path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "No se pudo abrir la imagen: %s", image_path);
        free(img_buf);
        return ESP_FAIL;
    }
    size_t bytes_read = fread(img_buf, 1, buf_size, f);
    fclose(f);

    if (bytes_read != buf_size) {
        ESP_LOGW(TAG, "El tamaño del fichero de imagen (%d) no coincide con el esperado (%d).", (int)bytes_read, (int)buf_size);
    }

    // 5. Volcar el búfer directamente al display (sin LVGL)
    ESP_LOGI(TAG, "Renderizando imagen nativa en el display...");
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, h_res, v_res, img_buf);

    // 6. Liberar la memoria
    free(img_buf);
    
    ESP_LOGI(TAG, "Pantalla de servicio mostrada con éxito.");
    return ESP_OK;
}