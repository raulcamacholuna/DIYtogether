/*
 * Fichero: ./components/diymon_ui/diymon_ui_helpers.c
 * Fecha: 13/08/2025 - 05:41 
 * Último cambio: Modificado para usar el fondo de pantalla desde el firmware.
 * Descripción: Se ha reescrito la función `ui_helpers_load_background` para que utilice el asset `bg_0` compilado en el firmware en lugar de cargarlo desde la tarjeta SD. Esto elimina la necesidad de un buffer en RAM para el fondo, solucionando el error de memoria insuficiente. La función para liberar el buffer ha sido eliminada.
 */
#include "diymon_ui_helpers.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assets/BG.h" // Incluir el nuevo asset de fondo

#define SD_MOUNT_POINT "/sdcard"
static const char* TAG_HELPERS = "UI_HELPERS";

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

// Construye la ruta a un asset de animación en la SD.
void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename) {
    char dir_name[9];
    get_evolution_dir_name(dir_name, sizeof(dir_name));
    snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
}

// Carga la imagen de fondo desde el firmware.
void ui_helpers_load_background(lv_obj_t* parent) {
    ESP_LOGI(TAG_HELPERS, "Fondo de pantalla cargado desde firmware.");
    lv_obj_set_style_bg_img_src(parent, &bg_0, 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
}

// La función para liberar el buffer de fondo ya no es necesaria.
void ui_helpers_free_background_buffer() {
    // Esta función se mantiene vacía por compatibilidad, pero ya no hace nada.
    ESP_LOGD(TAG_HELPERS, "ui_helpers_free_background_buffer ya no es necesaria y ha sido ignorada.");
}

// Implementaciones placeholder para que compile
void ui_helpers_create_diymon_gif(lv_obj_t* parent) {}
void ui_helpers_free_gif_buffer() {}
