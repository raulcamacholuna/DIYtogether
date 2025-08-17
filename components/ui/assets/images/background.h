/* Fecha: 17/08/2025 - 03:08  */
/* Fichero: components/ui/assets/images/background.h */
/* Último cambio: Movido desde 'components/ui/' como parte de la refactorización de assets. */
/* Descripción: Contiene el fondo de pantalla (BG.bin) como un array de C. Su centralización en 'assets/images' desacopla la apariencia de la lógica de la UI. */

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <lvgl.h>

// Contenido del fichero BG.bin (evolución 0)
static const uint8_t bg_0_map[] = {
    // Aquí debería ir el contenido real del fichero BG.bin convertido a un array C.
    // Por ahora, usamos datos de relleno para que compile.
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

// Descriptor de imagen LVGL que apunta al array del fondo.
const lv_img_dsc_t bg_0 = {
    .header.cf = LV_COLOR_FORMAT_RGB565,
    .header.w = 170,
    .header.h = 320,
    .header.stride = 170 * 2,
    .data_size = sizeof(bg_0_map),
    .data = bg_0_map,
};

#endif // BACKGROUND_H
