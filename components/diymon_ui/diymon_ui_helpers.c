/*
 * Fichero: ./components/diymon_ui/diymon_ui_helpers.c
 * Fecha: 10/08/2025 - 01:45
 * Último cambio: Modificada la carga del fondo para usar un fotograma estático.
 * Descripción: La función `ui_helpers_load_background` se ha adaptado para cargar una imagen específica (`idle/idle1f.bin`) que actúa como fondo a pantalla completa, incluyendo el personaje. Esto sirve como base para la futura implementación de la animación.
 */
#include "diymon_ui_helpers.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char *TAG = "UI_HELPERS";

// El conversor de LVGL añade un encabezado de 12 bytes al archivo .bin.
#define LVGL_BIN_HEADER_SIZE 12
// Se define el punto de montaje de la SD para mantener la consistencia.
#define SD_MOUNT_POINT "/sdcard"

// Búferes globales para almacenar los datos de imagen leídos desde la SD.
static uint8_t* g_img_data_buffer = NULL;
static lv_image_dsc_t g_bg_image_dsc;

static uint8_t* g_gif_data_buffer = NULL;
static lv_image_dsc_t g_gif_image_dsc;


void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename) {
    if (strncmp(asset_filename, "ICON_", 5) == 0) {
        snprintf(buffer, buffer_size, "%s/buttons/%s", SD_MOUNT_POINT, asset_filename);
    } else {
        const char* evo_code = diymon_get_current_code();
        char dir_name[9] = {0};
        int j = 0;
        for (int i = 0; evo_code[i] != '\0' && j < 8; i++) {
            if (evo_code[i] != '.') {
                dir_name[j++] = evo_code[i];
            }
        }
        snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
    }
}


void ui_helpers_load_background(lv_obj_t* parent) {
    if (!parent) {
        ESP_LOGE(TAG, "El objeto padre es nulo. No se puede cargar el fondo.");
        return;
    }

    char path_buffer[128];
    // --- CAMBIO: Apuntamos a la carpeta 'idle' y al fichero 'idle1f.bin' ---
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "idle/idle1f.bin");

    ESP_LOGI(TAG, "Intentando leer fondo desde la ruta VFS: '%s'", path_buffer);
    
    FILE* f = fopen(path_buffer, "rb"); 
    if (f == NULL) {
        ESP_LOGE(TAG, "FALLO: No se pudo abrir %s. Usando fondo rojo de emergencia.", path_buffer);
        lv_obj_set_style_bg_color(parent, lv_color_hex(0xff0000), 0);
        return;
    }

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size <= LVGL_BIN_HEADER_SIZE) {
        ESP_LOGE(TAG, "El archivo de fondo es demasiado pequeño para ser una imagen válida.");
        fclose(f);
        return;
    }

    ui_helpers_free_background_buffer();
    g_img_data_buffer = (uint8_t*)malloc(file_size);
    if (g_img_data_buffer == NULL) {
        ESP_LOGE(TAG, "Fallo al asignar memoria para el fondo.");
        fclose(f);
        return;
    }

    fread(g_img_data_buffer, 1, file_size, f);
    fclose(f);

    uint8_t* pixel_data = g_img_data_buffer + LVGL_BIN_HEADER_SIZE;
    size_t pixel_data_size = file_size - LVGL_BIN_HEADER_SIZE;
    for (size_t i = 0; i < pixel_data_size; i += 2) {
        uint8_t temp = pixel_data[i];
        pixel_data[i] = pixel_data[i+1];
        pixel_data[i+1] = temp;
    }

    // --- CAMBIO: Aseguramos que las dimensiones coinciden con la imagen a pantalla completa ---
    g_bg_image_dsc.header.w = 170;
    g_bg_image_dsc.header.h = 320;
    g_bg_image_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
    g_bg_image_dsc.data = pixel_data;
    g_bg_image_dsc.data_size = pixel_data_size;

    lv_obj_t* bg_img = lv_image_create(parent);
    if (!bg_img) {
        ESP_LOGE(TAG, "Fallo al crear el objeto de imagen para el fondo.");
        return;
    }
    lv_image_set_src(bg_img, &g_bg_image_dsc);
    // Posicionamos la imagen en la esquina para que ocupe toda la pantalla
    lv_obj_set_pos(bg_img, 0, 0);
    
    lv_obj_remove_flag(bg_img, LV_OBJ_FLAG_CLICKABLE);

    // Lo movemos al fondo del todo por si en el futuro se añaden botones.
    lv_obj_move_background(bg_img);

    ESP_LOGI(TAG, "ÉXITO: Fondo creado como un widget de imagen explícito desde %s", path_buffer);
}

void ui_helpers_create_diymon_gif(lv_obj_t* parent) {
#if LV_USE_GIF
    if (!parent) {
        ESP_LOGE(TAG, "El objeto padre es nulo. No se puede crear el GIF del DIYMON.");
        return;
    }

    char path_buffer[128];
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "diymon.gif");
    ESP_LOGI(TAG, "Intentando cargar GIF desde: '%s'", path_buffer);

    FILE* f = fopen(path_buffer, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "FALLO: No se pudo abrir el fichero del GIF en %s", path_buffer);
        return;
    }

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size == 0) {
        ESP_LOGE(TAG, "El fichero GIF está vacío.");
        fclose(f);
        return;
    }

    ui_helpers_free_gif_buffer();
    g_gif_data_buffer = (uint8_t*)malloc(file_size);
    if (g_gif_data_buffer == NULL) {
        ESP_LOGE(TAG, "FALLO: No se pudo asignar memoria (%d bytes) para el GIF.", file_size);
        fclose(f);
        return;
    }

    size_t bytes_read = fread(g_gif_data_buffer, 1, file_size, f);
    fclose(f);

    if (bytes_read != file_size) {
        ESP_LOGE(TAG, "FALLO: Error al leer el fichero GIF completo.");
        ui_helpers_free_gif_buffer();
        return;
    }

    g_gif_image_dsc.header.w = 0;
    g_gif_image_dsc.header.h = 0;
    g_gif_image_dsc.header.cf = LV_COLOR_FORMAT_UNKNOWN;
    g_gif_image_dsc.data = g_gif_data_buffer;
    g_gif_image_dsc.data_size = file_size;

    lv_obj_t *gif = lv_gif_create(parent);
    lv_gif_set_src(gif, &g_gif_image_dsc);
    lv_obj_align(gif, LV_ALIGN_CENTER, 0, 0);

    lv_obj_remove_flag(gif, LV_OBJ_FLAG_CLICKABLE);

    ESP_LOGI(TAG, "ÉXITO: Objeto GIF del DIYMON creado desde búfer de memoria.");

#else
    ESP_LOGW(TAG, "Soporte para GIF deshabilitado (LV_USE_GIF = 0). No se puede mostrar el GIF del DIYMON.");
#endif
}

void ui_helpers_free_background_buffer() {
    if (g_img_data_buffer != NULL) {
        free(g_img_data_buffer);
        g_img_data_buffer = NULL;
    }
}

void ui_helpers_free_gif_buffer() {
    if (g_gif_data_buffer != NULL) {
        free(g_gif_data_buffer);
        g_gif_data_buffer = NULL;
    }
}