/* Fecha: 17/08/2025 - 05:45  */
/* Fichero: components/ui/assets/images/ui_assets.c */
/* Último cambio: Corregido el valor del 'stride' en los descriptores de imagen a `ancho * 2`. */
/* Descripción: El ruido visual en los botones se debía a un 'stride' (bytes por fila) incorrecto. Para el formato LV_COLOR_FORMAT_RGB565A8, LVGL espera que el stride se refiera únicamente a la parte de color (RGB565 = 2 bytes/píxel), no al total de 3 bytes/píxel que incluye el canal alfa. El valor anterior (`ancho * 3`) causaba que LVGL desalineara la lectura de los datos de píxeles. La corrección a `ancho * 2` alinea la lectura correctamente y soluciona el problema de renderizado. */

#include "lvgl.h"

// --- NOTA IMPORTANTE ---
// Los siguientes arrays son solo placeholders para que el proyecto compile.
// Debes generar los ficheros .c para cada uno de tus iconos (ej: BTN_1.bin)
// con el conversor de LVGL (formato C array, RGB565A8) y enlazar este proyecto
// con ellos para que los datos reales reemplacen estos placeholders.

static const uint8_t asset_btn_1_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_2_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_3_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_4_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_5_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_6_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_7_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_8_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_btn_9_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_evo_1_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_evo_2_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_evo_3_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_evo_4_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t asset_evo_5_map[] = { 0xDE, 0xAD, 0xBE, 0xEF };


// --- Descriptores de Imagen Corregidos ---
// Stride = width * 2 bytes (solo para la parte de color RGB565)
// Data Size = width * height * 3 bytes (RGB565 + A8)

const lv_img_dsc_t asset_btn_1 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_1_map };
const lv_img_dsc_t asset_btn_2 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_2_map };
const lv_img_dsc_t asset_btn_3 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_3_map };
const lv_img_dsc_t asset_btn_4 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_4_map };
const lv_img_dsc_t asset_btn_5 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_5_map };
const lv_img_dsc_t asset_btn_6 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_6_map };
const lv_img_dsc_t asset_btn_7 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_7_map };
const lv_img_dsc_t asset_btn_8 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_8_map };
const lv_img_dsc_t asset_btn_9 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_btn_9_map };
const lv_img_dsc_t asset_evo_1 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_evo_1_map };
const lv_img_dsc_t asset_evo_2 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_evo_2_map };
const lv_img_dsc_t asset_evo_3 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_evo_3_map };
const lv_img_dsc_t asset_evo_4 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_evo_4_map };
const lv_img_dsc_t asset_evo_5 = { .header.cf = LV_COLOR_FORMAT_RGB565A8, .header.w = 50, .header.h = 50, .header.stride = 100, .data_size = 7500, .data = asset_evo_5_map };
