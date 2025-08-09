/*
 * Fichero: ./components/diymon_ui/animation_loader.c
 * Fecha: 10/08/2025 - 02:35
 * Último cambio: Adaptado el formato del nombre de fichero a `[prefijo][numero]f.bin`.
 * Descripción: La función `animation_loader_load_frame` se ha modificado para construir correctamente el nombre de los ficheros de animación (`idle1f.bin`, `idle2f.bin`, etc.), añadiendo la letra 'f' antes de la extensión.
 */
#include "animation_loader.h"
#include "esp_log.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "ANIM_LOADER";
#define LVGL_BIN_HEADER_SIZE 12

static void swap_bytes_for_rgb565(uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; i += 2) {
        uint8_t temp = data[i];
        data[i] = data[i + 1];
        data[i + 1] = temp;
    }
}

animation_t animation_loader_init(const char *path, uint16_t width, uint16_t height, uint16_t num_frames) {
    animation_t anim = { 0 };
    anim.base_path = strdup(path);
    anim.frame_count = num_frames;
    anim.width = width;
    anim.height = height;
    
    uint32_t line_bytes = width * 2; // 2 bytes por píxel para RGB565
    uint32_t stride = (line_bytes + 3) & ~3;
    size_t buffer_size = stride * height;

    anim.img_dsc.data = (uint8_t *)malloc(buffer_size);
    if (!anim.img_dsc.data) { ESP_LOGE(TAG, "Fallo al reservar buffer!"); animation_loader_free(&anim); return anim; }
    
    anim.img_dsc.header.w = width;
    anim.img_dsc.header.h = height;
    anim.img_dsc.header.stride = stride;
    anim.img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
    anim.img_dsc.data_size = buffer_size;
    
    ESP_LOGI(TAG, "Gestor de animación inicializado. Buffer de %d bytes.", (int)buffer_size);
    return anim;
}

bool animation_loader_load_frame(animation_t *anim, uint16_t frame_index, const char *prefix) {
    if (!anim) return false;
    char full_path[128];
    // --- CAMBIO: Se añade la 'f' al formato para que coincida con "idle1f.bin", "idle2f.bin", etc. ---
    snprintf(full_path, sizeof(full_path), "%s/%s%df.bin", anim->base_path, prefix, frame_index + 1);

    FILE *f = fopen(full_path, "rb");
    if (!f) { ESP_LOGE(TAG, "No se pudo abrir: %s", full_path); return false; }
    
    fseek(f, LVGL_BIN_HEADER_SIZE, SEEK_SET);
    fread((void *)anim->img_dsc.data, 1, anim->img_dsc.data_size, f);
    fclose(f);

    swap_bytes_for_rgb565((uint8_t *)anim->img_dsc.data, anim->img_dsc.data_size);
    return true;
}

void animation_loader_free(animation_t *anim) {
    if (!anim) return;
    if(anim->base_path) free(anim->base_path);
    if (anim->img_dsc.data) free((void*)anim->img_dsc.data);
    anim->frame_count = 0;
}