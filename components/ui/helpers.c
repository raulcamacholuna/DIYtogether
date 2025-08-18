/* Fecha: 17/08/2025 - 10:18  */
/* Fichero: components/ui/helpers.c */
/* Último cambio: Renombrado de 'diymon_ui_helpers.c' y actualizada la inclusión de la cabecera. */
/* Descripción: Implementación de las funciones de ayuda de la UI. Se ha renombrado el fichero y se ha actualizado la directiva #include para que apunte a 'helpers.h', completando la estandarización de nombres. */

#include "helpers.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assets/images/ui_assets.h"

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
    snprintf(buffer, buffer_size, "S:/diymon/%s/%s/", dir_name, asset_filename);
}

// Carga la imagen de fondo desde el firmware creando un objeto de imagen.
void ui_helpers_load_background(lv_obj_t* parent) {
    ESP_LOGI(TAG_HELPERS, "Creando objeto de imagen para el fondo desde firmware.");
    lv_obj_t *bg_img = lv_image_create(parent);
    lv_image_set_src(bg_img, &bg_0); // Usa el asset declarado en ui_assets.h
    lv_obj_set_pos(bg_img, 0, 0); 
    lv_obj_move_background(bg_img); 
}

// La función para liberar el buffer de fondo ya no es necesaria.
void ui_helpers_free_background_buffer(void) {
    ESP_LOGD(TAG_HELPERS, "ui_helpers_free_background_buffer ya no es necesaria y ha sido ignorada.");
}

// Implementaciones placeholder para que compile
void ui_helpers_create_diymon_gif(lv_obj_t* parent) {}
void ui_helpers_free_gif_buffer(void) {}
