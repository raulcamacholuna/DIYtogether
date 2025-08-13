/*
 * Fichero: ./components/diymon_ui/images.c
 * Fecha: 13/08/2025 - 11:27 
 * Último cambio: Creado para definir el asset de imagen de fondo.
 * Descripción: Fichero de assets de imagen. Proporciona la definición del símbolo 'bg_0'
 *              que el enlazador no encontraba. Se utiliza una imagen placeholder de 1x1 píxel
 *              para solucionar el error de compilación.
 */
#include "lvgl.h"

// --- IMAGEN PLACEHOLDER PARA SOLUCIONAR EL ERROR DE ENLAZADO ---
// Esto define un pixel negro (0x0000 en RGB565)
static const uint8_t bg_0_map[] = { 0x00, 0x00 };

// Descriptor de la imagen para LVGL
const lv_img_dsc_t bg_0 = {
    .header.cf = LV_COLOR_FORMAT_RGB565,
    .header.w = 1,
    .header.h = 1,
    .data_size = 2,
    .data = bg_0_map,
};
