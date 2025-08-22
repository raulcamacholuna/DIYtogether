/*
Fichero: components/ui/assets/images/RSPLS.h
Último cambio: Creación del fichero.
Descripción: Declara el descriptor de la imagen de fondo para el modo performance (RSPLS), para poder desacoplarlo de la lógica.
*/

#ifndef RSPLS_H
#define RSPLS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

extern const lv_img_dsc_t RSPLS;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // RSPLS_H
