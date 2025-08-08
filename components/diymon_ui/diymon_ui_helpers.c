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

// Búfer global para almacenar los datos de la imagen de fondo leída desde la SD.
static uint8_t* g_img_data_buffer = NULL;
// Descriptor de imagen de LVGL que se usará para el fondo.
static lv_image_dsc_t g_bg_image_dsc;


void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename) {
    // Comprueba si el recurso solicitado es un icono de botón.
    // strncmp es más seguro que strcmp para evitar desbordamientos.
    if (strncmp(asset_filename, "ICON_", 5) == 0) {
        // Si el nombre del fichero comienza con "ICON_", es un icono de botón.
        // La ruta para estos iconos es fija.
        snprintf(buffer, buffer_size, "%s/buttons/%s", SD_MOUNT_POINT, asset_filename);
    } else {
        // De lo contrario, es un asset de estado/animación dependiente de la evolución.
        // 1. Obtener el código de evolución actual (ej: "1.1.1").
        const char* evo_code = diymon_get_current_code();
        
        // 2. Convertir el código a un nombre de directorio compatible (ej: "111").
        char dir_name[9] = {0};
        int j = 0;
        for (int i = 0; evo_code[i] != '\0' && j < 8; i++) {
            if (evo_code[i] != '.') {
                dir_name[j++] = evo_code[i];
            }
        }

        // 3. Construir la ruta final usando el directorio de la evolución.
        snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
    }
}


void ui_helpers_load_background(lv_obj_t* parent) {
    if (!parent) {
        ESP_LOGE(TAG, "El objeto padre es nulo. No se puede cargar el fondo.");
        return;
    }

    char path_buffer[128];
    // Se construye la ruta al fichero de fondo, que debe llamarse "BG.BIN".
    // Esta función ahora usará la ruta de evolución correcta.
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "BG.BIN");

    ESP_LOGI(TAG, "Intentando leer fondo desde la ruta VFS: '%s'", path_buffer);
    
    FILE* f = fopen(path_buffer, "rb"); 
    if (f == NULL) {
        ESP_LOGE(TAG, "FALLO: No se pudo abrir %s. Usando fondo rojo de emergencia.", path_buffer);
        lv_obj_set_style_bg_color(parent, lv_color_hex(0xff0000), 0);
        lv_obj_set_style_bg_img_src(parent, NULL, 0);
        return;
    }

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size <= LVGL_BIN_HEADER_SIZE) {
        ESP_LOGE(TAG, "El archivo es demasiado pequeño para ser una imagen válida.");
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

    // Se realiza el intercambio de bytes para corregir los colores del formato RGB565.
    uint8_t* pixel_data = g_img_data_buffer + LVGL_BIN_HEADER_SIZE;
    size_t pixel_data_size = file_size - LVGL_BIN_HEADER_SIZE;
    for (size_t i = 0; i < pixel_data_size; i += 2) {
        uint8_t temp = pixel_data[i];
        pixel_data[i] = pixel_data[i+1];
        pixel_data[i+1] = temp;
    }

    // Se configura el descriptor de imagen de LVGL con los datos leídos.
    g_bg_image_dsc.header.w = 170;
    g_bg_image_dsc.header.h = 320;
    g_bg_image_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
    g_bg_image_dsc.data = pixel_data;
    g_bg_image_dsc.data_size = pixel_data_size;

    // Se establece la imagen como fondo del objeto padre.
    lv_obj_set_style_bg_img_src(parent, &g_bg_image_dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
    ESP_LOGI(TAG, "ÉXITO: Fondo cargado y aplicado desde %s", path_buffer);
}


void ui_helpers_free_background_buffer() {
    if (g_img_data_buffer != NULL) {
        free(g_img_data_buffer);
        g_img_data_buffer = NULL;
    }
}