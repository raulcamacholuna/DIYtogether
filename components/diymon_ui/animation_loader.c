/*
 * Fichero: ./components/diymon_ui/animation_loader.c
 * Fecha: 13/08/2025 - 05:59 
 * Último cambio: Revertido el búfer de animación a una gestión sin buffer compartido.
 * Descripción: Se ha revertido la lógica para que cada animación (idle y acción) gestione su propio búfer. El búfer compartido causaba problemas de memoria y complejidad. Ahora, nimation_loader_init reserva memoria y nimation_loader_free la libera, simplificando el ciclo de vida de las animaciones.
 */
#include "animation_loader.h"
#include "esp_log.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "ANIM_LOADER";
#define LVGL_BIN_HEADER_SIZE 12

animation_t animation_loader_init(const char *path, uint16_t width, uint16_t height, uint16_t num_frames) {
    animation_t anim = { 0 };
    anim.base_path = path ? strdup(path) : NULL;
    anim.frame_count = num_frames;
    anim.width = width;
    anim.height = height;
    
    uint32_t line_bytes = width * 2;
    uint32_t stride = (line_bytes + 3) & ~3;
    size_t buffer_size = stride * height;

    anim.img_dsc.data = (uint8_t *)malloc(buffer_size);
    if (!anim.img_dsc.data) { 
        ESP_LOGE(TAG, "Fallo al reservar buffer de animación de tamaño %d!", (int)buffer_size);
        animation_loader_free(&anim); 
        return anim; 
    }
    
    anim.img_dsc.header.w = width;
    anim.img_dsc.header.h = height;
    anim.img_dsc.header.stride = stride;
    anim.img_dsc.header.cf = LV_COLOR_FORMAT_RGB565A8;
    anim.img_dsc.data_size = buffer_size;
    
    ESP_LOGI(TAG, "Gestor de animación inicializado. Buffer de %d bytes.", (int)buffer_size);
    return anim;
}

bool animation_loader_load_frame(animation_t *anim, uint16_t frame_index, const char *prefix) {
    if (!anim || !anim->base_path || !anim->img_dsc.data) return false;
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "%s/%s%d.bin", anim->base_path, prefix, frame_index + 1);

    FILE *f = fopen(full_path, "rb");
    if (!f) { return false; } 
    
    fseek(f, LVGL_BIN_HEADER_SIZE, SEEK_SET);
    fread((void *)anim->img_dsc.data, 1, anim->img_dsc.data_size, f);
    fclose(f);

    return true;
}

void animation_loader_free(animation_t *anim) {
    if (!anim) return;
    if(anim->base_path) {
        free(anim->base_path);
        anim->base_path = NULL;
    }
    if (anim->img_dsc.data) {
        free((void*)anim->img_dsc.data);
        anim->img_dsc.data = NULL;
    }
    anim->frame_count = 0;
}

uint16_t animation_loader_count_frames(const char *path, const char *prefix) {
    if (!path || !prefix) {
        return 0;
    }

    uint16_t count = 0;
    DIR *dir = opendir(path);
    if (!dir) {
        ESP_LOGE(TAG, "No se pudo abrir el directorio: %s", path);
        return 0;
    }

    struct dirent *ent;
    size_t prefix_len = strlen(prefix);

    while ((ent = readdir(dir)) != NULL) {
        if (strncmp(ent->d_name, prefix, prefix_len) == 0) {
            count++;
        }
    }

    closedir(dir);
    return count;
}
