/* Fecha: 17/08/2025 - 10:15  */
/* Fichero: components/ui/assets/images/ui_assets.h */
/* Último cambio: Centralizadas las declaraciones de TODOS los assets de imagen del firmware. */
/* Descripción: Este fichero de cabecera ahora sirve como el único punto de declaración para todos los assets gráficos compilados en el firmware (fondos de pantalla e iconos). Al centralizar las declaraciones de 'bg_0', 'bg_config' y todos los 'asset_btn_*', se simplifica la gestión de recursos y se soluciona la fragmentación que causaba confusión y errores de compilación. */

#ifndef UI_ASSETS_H
#define UI_ASSETS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Assets de Fondos de Pantalla ---
extern const lv_img_dsc_t bg_0;      // Fondo principal de la aplicación.
extern const lv_img_dsc_t bg_config; // Fondo para las pantallas de servicio/configuración.

// --- Assets de Iconos de Botones (BTN) ---
extern const lv_img_dsc_t asset_btn_1; // Comer
extern const lv_img_dsc_t asset_btn_2; // Ejercicio
extern const lv_img_dsc_t asset_btn_3; // Atacar
extern const lv_img_dsc_t asset_btn_4; // Brillo
extern const lv_img_dsc_t asset_btn_5; // Apagar Pantalla
extern const lv_img_dsc_t asset_btn_6; // Placeholder Admin (antes Modo Config)
extern const lv_img_dsc_t asset_btn_7; // Reset Total
extern const lv_img_dsc_t asset_btn_8; // Servidor Ficheros
extern const lv_img_dsc_t asset_btn_9; // Placeholder Config

// --- Assets de Iconos de Evolución (EVO) ---
extern const lv_img_dsc_t asset_evo_1; // Fuego
extern const lv_img_dsc_t asset_evo_2; // Agua
extern const lv_img_dsc_t asset_evo_3; // Tierra
extern const lv_img_dsc_t asset_evo_4; // Viento
extern const lv_img_dsc_t asset_evo_5; // Involucionar


#ifdef __cplusplus
}
#endif

#endif // UI_ASSETS_H
