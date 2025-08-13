/*
 * Fichero: ./components/diymon_ui/diymon_ui_helpers.c
 * Fecha: 13/08/2025 - 11:45 
 * Último cambio: Corregido el tipo de la cabecera de imagen de LVGL.
 * Descripción: Se ha corregido el nombre del tipo de la cabecera de imagen
 *              de 'lv_img_header_t' a 'lv_image_header_t' para que coincida
 *              con la API de LVGL y solucionar el error de compilación.
 */
#include "diymon_ui_helpers.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SD_MOUNT_POINT "/sdcard"
static const char* TAG_HELPERS = "UI_HELPERS";

// Buffer global para la imagen de fondo cargada desde la SD
static uint8_t* g_bg_buffer = NULL;
static lv_img_dsc_t g_bg_dsc;

// Función interna para obtener el nombre del directorio de evolución (ej: "1.1.1" -> "111")
static void get_evolution_dir_name(char* dir_name_buffer, size_t buffer_size) {
    const char* evo_code = diymon_get_current_code();
    memset(dir_name_buffer, 0, buffer_size);
    int j = 0;
    for (int i = 0; evo_code[i] != '\0' && j < buffer_size - 1; i++) {
        if (evo_code[i] != '.') {
            dir_name_buffer[j++] = evo_code[i];
        }
    }
}

// Construye la ruta a un asset, corrigiendo el caso por defecto.
void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename) {
    char dir_name[9];
    get_evolution_dir_name(dir_name, sizeof(dir_name));

    if (strncmp(asset_filename, "BTN_", 4) == 0 ||
        strncmp(asset_filename, "EVO_", 4) == 0) {
        snprintf(buffer, buffer_size, "%s/buttons/%s", SD_MOUNT_POINT, asset_filename);
    } else {
        // Caso por defecto corregido: para todos los demás assets (BG, IDLE, etc.)
        snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
    }
}

// Carga la imagen de fondo desde la SD.
void ui_helpers_load_background(lv_obj_t* parent) {
    char path[128];
    ui_helpers_build_asset_path(path, sizeof(path), "BG.bin");

    FILE* f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG_HELPERS, "No se pudo abrir el fondo: %s. Se usará fondo negro.", path);
        lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= sizeof(lv_image_header_t)) {
        ESP_LOGE(TAG_HELPERS, "El fichero de fondo es inválido o está vacío: %s", path);
        fclose(f);
        lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
        return;
    }

    ui_helpers_free_background_buffer(); // Liberar buffer anterior si existe

    size_t data_size = size - sizeof(lv_image_header_t);
    g_bg_buffer = (uint8_t*)malloc(data_size);
    if (!g_bg_buffer) {
        ESP_LOGE(TAG_HELPERS, "Fallo al reservar %d bytes para el fondo.", (int)data_size);
        fclose(f);
        return;
    }

    fread(&g_bg_dsc.header, 1, sizeof(lv_image_header_t), f);
    fread(g_bg_buffer, 1, data_size, f);
    fclose(f);

    g_bg_dsc.data_size = data_size;
    g_bg_dsc.data = g_bg_buffer;
    
    lv_obj_set_style_bg_img_src(parent, &g_bg_dsc, 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
    ESP_LOGI(TAG_HELPERS, "Fondo cargado desde %s", path);
}

// Libera la memoria del buffer de fondo.
void ui_helpers_free_background_buffer() {
    if (g_bg_buffer) {
        free(g_bg_buffer);
        g_bg_buffer = NULL;
        ESP_LOGI(TAG_HELPERS, "Buffer de fondo liberado.");
    }
}

// Implementaciones placeholder para que compile
void ui_helpers_create_diymon_gif(lv_obj_t* parent) {}
void ui_helpers_free_gif_buffer() {}
