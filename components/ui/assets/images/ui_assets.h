/* Fecha: 17/08/2025 - 03:07  */
/* Fichero: components/ui/assets/images/ui_assets.h */
/* Último cambio: Movido desde 'components/ui/' como parte de la refactorización de assets. */
/* Descripción: Este fichero de cabecera centraliza las declaraciones de todas las imágenes de los botones que se compilan en el firmware. Permite que el gestor de assets acceda a ellas de forma segura. */

#ifndef UI_ASSETS_H
#define UI_ASSETS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Declaración de Assets de Botones (BTN) ---
extern const lv_img_dsc_t asset_btn_1;
extern const lv_img_dsc_t asset_btn_2;
extern const lv_img_dsc_t asset_btn_3;
extern const lv_img_dsc_t asset_btn_4;
extern const lv_img_dsc_t asset_btn_5;
extern const lv_img_dsc_t asset_btn_6;
extern const lv_img_dsc_t asset_btn_7;
extern const lv_img_dsc_t asset_btn_8;
extern const lv_img_dsc_t asset_btn_9;

// --- Declaración de Assets de Evolución (EVO) ---
extern const lv_img_dsc_t asset_evo_1;
extern const lv_img_dsc_t asset_evo_2;
extern const lv_img_dsc_t asset_evo_3;
extern const lv_img_dsc_t asset_evo_4;
extern const lv_img_dsc_t asset_evo_5;


#ifdef __cplusplus
}
#endif

#endif // UI_ASSETS_H
