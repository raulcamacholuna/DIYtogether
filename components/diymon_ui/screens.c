/*
 * Archivo: screens.c
 * Ubicación: components/diymon_ui/
 * Versión: VICTORIA FINAL (Maneja el formato .bin de LVGL correctamente)
 */
#include "screens.h"
#include "lvgl.h"
#include "ui_priv.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

objects_t objects;
lv_obj_t *g_background_obj = NULL;
static uint8_t* g_img_data_buffer = NULL;
static lv_image_dsc_t g_bg_image_dsc;

static const char *TAG = "DIYMON_BIN_LOADER";

// [LA SOLUCIÓN]
// El conversor de LVGL añade un encabezado de 12 bytes al archivo .bin.
// Lo definimos aquí para tenerlo en cuenta.
#define LVGL_BIN_HEADER_SIZE 12

void ui_update_diymon_background(void) {
    if (!g_background_obj) {
        ESP_LOGE(TAG, "Objeto de fondo nulo!");
        return;
    }

    const char* evo_code = diymon_get_current_code(); // ej: "1.1.1"
    
    char path_version[32];
    int i = 0;
    for (i = 0; evo_code[i] != '\0' && i < sizeof(path_version) - 1; i++) {
        path_version[i] = (evo_code[i] == '.') ? '-' : evo_code[i];
    }
    path_version[i] = '\0';

    char bin_path[128];
    snprintf(bin_path, sizeof(bin_path), "/sdcard/diymon/%s/BG.bin", path_version);

    ESP_LOGI(TAG, "Intentando leer binario desde: '%s'", bin_path);
    FILE* f = fopen(bin_path, "rb");

    if (f == NULL) {
        ESP_LOGE(TAG, "¡FALLO! No se pudo abrir %s.", bin_path);
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xff0000), 0);
        return;
    }

    ESP_LOGI(TAG, "¡ÉXITO! Se abrió '%s'.", bin_path);

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // [LA SOLUCIÓN] Ya no comprobamos el tamaño exacto, solo que sea válido.
    if (file_size < LVGL_BIN_HEADER_SIZE) {
        ESP_LOGE(TAG, "El archivo es demasiado pequeño para ser una imagen válida.");
        fclose(f);
        return;
    }

    // Leemos el archivo completo (header + píxeles) en la memoria
    if (g_img_data_buffer != NULL) {
        free(g_img_data_buffer);
    }
    g_img_data_buffer = (uint8_t*)malloc(file_size);
    if (g_img_data_buffer == NULL) {
        ESP_LOGE(TAG, "Fallo al asignar memoria.");
        fclose(f);
        return;
    }

    fread(g_img_data_buffer, 1, file_size, f);
    fclose(f);
    

    uint8_t* pixel_data = g_img_data_buffer + LVGL_BIN_HEADER_SIZE;
    size_t pixel_data_size = file_size - LVGL_BIN_HEADER_SIZE;

    // Recorremos el búfer de píxeles de 2 en 2 bytes
    for (size_t i = 0; i < pixel_data_size; i += 2) {
        uint8_t temp = pixel_data[i];
        pixel_data[i] = pixel_data[i+1];
        pixel_data[i+1] = temp;
    }
    ESP_LOGI(TAG, "Se han intercambiado los bytes de %d píxeles.", pixel_data_size / 2);



    ESP_LOGI(TAG, "Leídos %d bytes. Configurando imagen cruda...", file_size);

    // [LA SOLUCIÓN] Configuramos el descriptor de LVGL para que apunte
    // a los datos de píxeles, saltándose el encabezado.
    g_bg_image_dsc.header.w = 170;
    g_bg_image_dsc.header.h = 320;
    g_bg_image_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
    g_bg_image_dsc.data = g_img_data_buffer + LVGL_BIN_HEADER_SIZE; // <-- Apuntamos 12 bytes más adelante
    g_bg_image_dsc.data_size = file_size - LVGL_BIN_HEADER_SIZE;    // <-- El tamaño de los datos es el total menos el header

    // Le decimos al objeto de imagen que use nuestro descriptor
    lv_image_set_src(g_background_obj, &g_bg_image_dsc);
}

// --- El resto de funciones no cambian ---
void create_screen_main() {
    lv_obj_t *screen = lv_obj_create(NULL);
    objects.main = screen;
    lv_obj_set_size(screen, 170, 320);
    lv_obj_center(screen);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    g_background_obj = lv_image_create(screen);
    lv_obj_align(g_background_obj, LV_ALIGN_CENTER, 0, 0);
}
void delete_screen_main() {
    if(objects.main) lv_obj_del(objects.main);
    if (g_img_data_buffer) {free(g_img_data_buffer); g_img_data_buffer = NULL;}
    memset(&objects, 0, sizeof(objects_t));
    g_background_obj = NULL;
}
void create_screens() {
    create_screen_main();
}