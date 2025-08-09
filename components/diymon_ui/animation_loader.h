/*
 * Fichero: ./components/diymon_ui/animation_loader.h
 * Fecha: 09/08/2025 - 21:05
 * Último cambio: Sin cambios. Restaurada la interfaz para la versión final.
 * Descripción: Define la interfaz para el cargador de animaciones bajo demanda,
 *              optimizado para el formato RGB565 con Chroma Keying.
 */
#ifndef ANIMATION_LOADER_H
#define ANIMATION_LOADER_H

#include "lvgl.h"

typedef struct {
    char *base_path;
    uint16_t frame_count;
    uint16_t width;
    uint16_t height;
    lv_img_dsc_t img_dsc;
} animation_t;

animation_t animation_loader_init(const char *path, uint16_t width, uint16_t height, uint16_t num_frames);
bool animation_loader_load_frame(animation_t *anim, uint16_t frame_index, const char *prefix);
void animation_loader_free(animation_t *anim);

#endif // ANIMATION_LOADER_H