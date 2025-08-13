/*
 * Fichero: ./components/diymon_ui/diymon_ui_helpers.c
 * Fecha: 13/08/2025 - 11:27 
 * Último cambio: Implementada la carga de fondo desde assets compilados.
 * Descripción: Se implementa la función 'ui_helpers_load_background' para que utilice
 *              el asset 'bg_0' compilado, resolviendo la referencia indefinida
 *              del enlazador.
 */
#include "diymon_ui_helpers.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include "images.h" // Incluir la definición del asset
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SD_MOUNT_POINT "/sdcard"

// Función interna para obtener el nombre del directorio de evolución (ej: "111")
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

void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename) {
    char dir_name[9];
    get_evolution_dir_name(dir_name, sizeof(dir_name));

    if (strncmp(asset_filename, "BTN_", 4) == 0 ||
        strncmp(asset_filename, "EVO_", 4) == 0) {
        snprintf(buffer, buffer_size, "%s/buttons/%s", SD_MOUNT_POINT, asset_filename);
    }
    else if (strncmp(asset_filename, "IDLE_", 5) == 0) {
         snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
    }
    else if (strncmp(asset_filename, "EAT_", 4) == 0 || strncmp(asset_filename, "GYM_", 4) == 0 || strncmp(asset_filename, "ATK_", 4) == 0) {
         snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
    }
    else {
        snprintf(buffer, buffer_size, "%s/DIYMON/%s/", SD_MOUNT_POINT, dir_name);
    }
}

// Implementación de la función que faltaba y causaba el error de enlazado
void ui_helpers_load_background(lv_obj_t* parent) {
    if (parent) {
        // Usa el asset 'bg_0' que ahora está definido en images.c
        lv_obj_set_style_bg_img_src(parent, &bg_0, 0);
    }
}

// Las otras funciones se dejan como estaban, ya que su lógica no está implicada en el error.
void ui_helpers_free_background_buffer() {
    // Implementación si fuera necesaria para un búfer dinámico.
}

void ui_helpers_create_diymon_gif(lv_obj_t* parent) {
    // Implementación para crear el GIF.
}

void ui_helpers_free_gif_buffer() {
    // Implementación para liberar el búfer del GIF.
}
