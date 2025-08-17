/* Fecha: 17/08/2025 - 03:24  */
/* Fichero: components/ui/assets/images/ui_assets.c */
/* Último cambio: Creado para proporcionar las definiciones de los assets de imagen y resolver el error del enlazador. */
/* Descripción: Este fichero contiene las definiciones reales de los descriptores de imagen LVGL para cada icono. El enlazador fallaba porque solo encontraba las declaraciones (extern) en los ficheros de cabecera. Al proporcionar aquí las definiciones, se resuelve el error 'undefined reference'. */

#include "lvgl.h"

// --- Definiciones Placeholder para los Assets de Botones (BTN) ---
// NOTA: Estos son datos de relleno. Deben ser reemplazados por los datos reales
// generados por el conversor de imágenes de LVGL.

static const uint8_t placeholder_map[] = {0x00}; // 1x1 pixel negro

const lv_img_dsc_t asset_btn_1 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_2 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_3 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_4 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_5 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_6 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_7 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_8 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_btn_9 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };

// --- Definiciones Placeholder para los Assets de Evolución (EVO) ---
const lv_img_dsc_t asset_evo_1 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_evo_2 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_evo_3 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_evo_4 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };
const lv_img_dsc_t asset_evo_5 = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };

// --- Definiciones Placeholder para los Fondos ---
const lv_img_dsc_t bg_config = { .header.w = 1, .header.h = 1, .data_size = 1, .header.cf = LV_COLOR_FORMAT_RGB565, .data = placeholder_map, };

