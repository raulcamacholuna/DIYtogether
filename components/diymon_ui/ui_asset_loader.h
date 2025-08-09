/*
  Fichero: ./components/diymon_ui/ui_asset_loader.h
  Fecha: 10/08/2025 - 00:30
  Último cambio: Creación del fichero.
  Descripción: Interfaz pública para el gestor de assets de la UI. Define las
               funciones para inicializar, liberar y obtener los recursos gráficos
               (iconos) que se precargan en memoria al inicio.
*/
#ifndef UI_ASSET_LOADER_H
#define UI_ASSET_LOADER_H

#include "lvgl.h"

// Identificadores únicos para cada asset precargado.
typedef enum {
    ASSET_ICON_EAT,
    ASSET_ICON_GYM,
    ASSET_ICON_ATK,
    ASSET_COUNT // Utilidad para contar el número de assets
} ui_asset_id_t;

/**
 * @brief Inicializa el gestor y precarga todos los assets en memoria.
 */
void ui_assets_init(void);

/**
 * @brief Libera la memoria utilizada por los assets precargados.
 */
void ui_assets_deinit(void);

/**
 * @brief Obtiene un puntero al descriptor de imagen de un icono ya cargado.
 * @param asset_id El identificador del icono a obtener.
 * @return Un puntero constante al descriptor de imagen LVGL.
 */
const lv_img_dsc_t* ui_assets_get_icon(ui_asset_id_t asset_id);

#endif // UI_ASSET_LOADER_H