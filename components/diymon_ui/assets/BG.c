/*
  Fichero: ./components/diymon_ui/assets/BG.c
  Fecha: 13/08/2025 - 05:41 
  Último cambio: Creado el asset del fondo de pantalla como un fichero C.
  Descripción: Contiene los datos de la imagen de fondo como un array C en formato I8 (indexado de 8 bits) para ser compilado directamente en el firmware. Esto elimina la necesidad de cargar la imagen desde la SD a la RAM.
*/
#include <lvgl.h>

// Contenido del fichero BG.bin (evolución 0) en formato I8
// NOTA: Este es un marcador de posición. El contenido real debe ser generado
//       por el convertidor de imágenes de LVGL a partir del fichero de imagen original.
static const uint8_t bg_0_map[] = {
    // Paleta de colores (ejemplo, hasta 256 colores)
    0x00, 0x00, 0x00, 0xFF, // Color 0: Negro
    0xFF, 0xFF, 0xFF, 0xFF, // Color 1: Blanco
    // ... más colores de la paleta ...

    // Datos de píxeles (índices a la paleta)
    0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 
    0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01
};

// Descriptor de imagen LVGL que apunta al array del fondo.
const lv_img_dsc_t bg_0 = {
    .header.cf = LV_COLOR_FORMAT_I8,
    .header.w = 170,
    .header.h = 320,
    .header.stride = 170,
    .data_size = sizeof(bg_0_map),
    .data = bg_0_map,
};
