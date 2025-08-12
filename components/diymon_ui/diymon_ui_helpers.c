/*
 * Fichero: ./components/diymon_ui/diymon_ui_helpers.c
 * Fecha: 14/08/2025 - 11:30 am
 * Último cambio: Añadido el prefijo "EVO_" a la lógica de búsqueda de iconos.
 * Descripción: Se ha corregido la función de construcción de rutas para que reconozca el prefijo "EVO_", permitiendo que los iconos del panel lateral se localicen correctamente en la carpeta `/buttons/`.
 */
#include "diymon_ui_helpers.h"
#include "diymon_evolution.h"
#include "esp_log.h"
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

    // --- LA SOLUCIÓN ---
    // Se añade la condición para el prefijo `EVO_`. Ahora, cualquier asset que
    // comience por `BTN_` o `EVO_` se buscará en la carpeta `/buttons/`.
    if (strncmp(asset_filename, "BTN_", 4) == 0 ||
        strncmp(asset_filename, "EVO_", 4) == 0) {
        snprintf(buffer, buffer_size, "%s/buttons/%s", SD_MOUNT_POINT, asset_filename);
    }
    // Ruta para las animaciones de idle (ej: /sdcard/DIYMON/111/IDLE_1.bin)
    else if (strncmp(asset_filename, "IDLE_", 5) == 0) {
         snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
    }
    // Ruta para animaciones de acción (ej: /sdcard/DIYMON/111/EAT_1.bin)
    else if (strncmp(asset_filename, "EAT_", 4) == 0 || strncmp(asset_filename, "GYM_", 4) == 0 || strncmp(asset_filename, "ATK_", 4) == 0) {
         snprintf(buffer, buffer_size, "%s/DIYMON/%s/%s", SD_MOUNT_POINT, dir_name, asset_filename);
    }
    // Caso por defecto: devuelve la ruta base de la evolución actual.
    else {
        snprintf(buffer, buffer_size, "%s/DIYMON/%s/", SD_MOUNT_POINT, dir_name);
    }
}