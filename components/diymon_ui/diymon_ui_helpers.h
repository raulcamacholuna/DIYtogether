/*
 * Fichero: ./components/diymon_ui/diymon_ui_helpers.h
 * Fecha: 13/08/2025 - 11:39 
 * Último cambio: Limpieza de includes.
 * Descripción: Funciones de ayuda para la interfaz de usuario. Se elimina
 *              el include a 'images.h' que ya no existe.
 */
#ifndef DIYMON_UI_HELPERS_H
#define DIYMON_UI_HELPERS_H

#include "lvgl.h"

void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename);
void ui_helpers_load_background(lv_obj_t* parent);
void ui_helpers_free_background_buffer();
void ui_helpers_create_diymon_gif(lv_obj_t* parent);
void ui_helpers_free_gif_buffer();

#endif // DIYMON_UI_HELPERS_H
