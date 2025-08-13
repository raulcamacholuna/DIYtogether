/*
 * Fichero: components/diymon_ui/diymon_ui_helpers.h
 * Fecha: 13/08/2025 - 11:27 
 * Último cambio: Añadida la inclusión de images.h para el fondo.
 * Descripción: Funciones de ayuda para la interfaz de usuario. Se incluye la
 *              declaración del asset de imagen 'bg_0'.
 */
#ifndef DIYMON_UI_HELPERS_H
#define DIYMON_UI_HELPERS_H

#include "lvgl.h"
#include "images.h" // Incluye la declaración de bg_0

void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename);
void ui_helpers_load_background(lv_obj_t* parent);
void ui_helpers_free_background_buffer();
void ui_helpers_create_diymon_gif(lv_obj_t* parent);
void ui_helpers_free_gif_buffer();

#endif // DIYMON_UI_HELPERS_H
