/*
 * Archivo: screens.c
 * Versión: 19.0 (Prueba Final de Subdirectorios)
 * Descripción: Intenta leer un archivo 8.3 desde un subdirectorio.
 */
#include "screens.h"
#include "lvgl.h"
#include "ui_priv.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <ctype.h>

objects_t objects;
lv_obj_t *g_background_obj = NULL;
static uint8_t* g_png_data_buffer = NULL;
static lv_image_dsc_t g_bg_image_dsc;

static const char *TAG = "DIYMON_SUBDIR_TEST";

void ui_update_diymon_background(void) {
    if (!g_background_obj) {
        ESP_LOGE(TAG, "Objeto de fondo nulo!");
        return;
    }

    const char* evo_code = diymon_get_current_code(); // ej: "1.1.1"
    
    // --- 1. Generar la versión para el nombre de la CARPETA (ej: "1-1-1") ---
    char path_version[32];
    int i = 0;
    for (i = 0; evo_code[i] != '\0' && i < sizeof(path_version) - 1; i++) {
        path_version[i] = (evo_code[i] == '.') ? '-' : evo_code[i];
    }
    path_version[i] = '\0';

    // --- 2. Generar la versión para el nombre del ARCHIVO (ej: "111") ---
    char numeric_version[32];
    int src_idx = 0;
    int dst_idx = 0;
    while (evo_code[src_idx] != '\0' && dst_idx < sizeof(numeric_version) - 1) {
        if (isdigit((unsigned char)evo_code[src_idx])) {
            numeric_version[dst_idx++] = evo_code[src_idx];
        }
        src_idx++;
    }
    numeric_version[dst_idx] = '\0';

    char png_path[128];
    // [CAMBIO CLAVE] Construimos la ruta completa al archivo dentro del subdirectorio
    snprintf(png_path, sizeof(png_path), "/sdcard/diymon/%s/E%sBG.PNG", path_version, numeric_version);

    ESP_LOGI(TAG, "Intentando leer desde subdirectorio: '%s'", png_path);
    FILE* f = fopen(png_path, "rb");

    if (f == NULL) {
        ESP_LOGE(TAG, "¡FALLO! No se pudo abrir %s.", png_path);
        ESP_LOGE(TAG, "Conclusión: El problema es el acceso a subdirectorios.");
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xff0000), 0);
        return;
    }

    ESP_LOGI(TAG, "¡¡¡VICTORIA!!! Se abrió '%s' desde un subdirectorio.", png_path);

    // ... (El resto del código de lectura es idéntico)
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (file_size == 0) { ESP_LOGE(TAG, "Archivo vacío."); fclose(f); return; }
    if (g_png_data_buffer != NULL) free(g_png_data_buffer);
    g_png_data_buffer = (uint8_t*)malloc(file_size);
    if (g_png_data_buffer == NULL) { ESP_LOGE(TAG, "Fallo de memoria."); fclose(f); return; }
    fread(g_png_data_buffer, 1, file_size, f);
    fclose(f);
    ESP_LOGI(TAG, "Leídos %d bytes. Entregando a LVGL.", file_size);
    g_bg_image_dsc.header.w = 0;
    g_bg_image_dsc.header.h = 0;
    g_bg_image_dsc.header.cf = LV_COLOR_FORMAT_UNKNOWN;
    g_bg_image_dsc.data = g_png_data_buffer;
    g_bg_image_dsc.data_size = file_size;
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
    if (g_png_data_buffer) {free(g_png_data_buffer); g_png_data_buffer = NULL;}
    memset(&objects, 0, sizeof(objects_t));
    g_background_obj = NULL;
}
void create_screens() {
    create_screen_main();
}