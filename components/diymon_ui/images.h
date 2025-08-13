// Fichero: ./components/diymon_ui/images.h
// Fecha: 13/08/2025 - 12:43 
// Último cambio: Adaptado para usar assets compilados desde ficheros .c.
// Descripción: Declara los descriptores de imagen como variables 'extern'. Estos símbolos son definidos en los ficheros .c correspondientes dentro de la carpeta 'assets', permitiendo al enlazador encontrarlos.

#ifndef IMAGES_H
#define IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- SÍMBOLOS DEFINIDOS EN LOS FICHEROS .c DE ASSETS ---
// Cada asset es ahora una variable global, no un símbolo binario.

// Panel Jugador
extern const lv_img_dsc_t BTN_1;
extern const lv_img_dsc_t BTN_2;
extern const lv_img_dsc_t BTN_3;

// Panel Admin
extern const lv_img_dsc_t BTN_4;
extern const lv_img_dsc_t BTN_5;
extern const lv_img_dsc_t BTN_6;

// Panel Config
extern const lv_img_dsc_t BTN_7;
extern const lv_img_dsc_t BTN_8;
extern const lv_img_dsc_t BTN_9;

// Panel Evolución
extern const lv_img_dsc_t EVO_1;
extern const lv_img_dsc_t EVO_2;
extern const lv_img_dsc_t EVO_3;
extern const lv_img_dsc_t EVO_4;
extern const lv_img_dsc_t EVO_5;

#ifdef __cplusplus
}
#endif

#endif // IMAGES_H
