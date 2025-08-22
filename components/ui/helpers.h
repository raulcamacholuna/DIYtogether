/* Fecha: 17/08/2025 - 10:18  */
/* Fichero: components/ui/helpers.h */
/* Último cambio: Renombrado de 'diymon_ui_helpers.h' para simplificar y estandarizar. */
/* Descripción: Funciones de ayuda para la interfaz de usuario. Proporciona utilidades para construir rutas de assets y cargar elementos visuales como el fondo de pantalla. */
#ifndef HELPERS_H
#define HELPERS_H

#include "lvgl.h"

extern lv_obj_t *g_wallpaper_img;

void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename);
void ui_helpers_load_background(lv_obj_t* parent);
void ui_helpers_free_background_buffer(void);
void ui_helpers_create_diymon_gif(lv_obj_t* parent);
void ui_helpers_free_gif_buffer(void);

#endif // HELPERS_H
