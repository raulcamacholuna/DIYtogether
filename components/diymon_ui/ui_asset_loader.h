/*
  Fichero: ./components/diymon_ui/ui_asset_loader.h
  Fecha: 12/08/2025 - 10:31
  Último cambio: Añadidos IDs para los iconos del panel de administración.
  Descripción: Interfaz pública para el gestor de assets. Define los identificadores únicos para todos los recursos gráficos (iconos) que se precargan en memoria.
*/
#ifndef UI_ASSET_LOADER_H
#define UI_ASSET_LOADER_H

#include "lvgl.h"

// Identificadores únicos para cada asset precargado.
typedef enum {
    // Iconos del panel de jugador
    ASSET_ICON_EAT,
    ASSET_ICON_GYM,
    ASSET_ICON_ATK,

    // Iconos del panel de administración
    ASSET_ICON_LVL_DOWN,
    ASSET_ICON_SCREEN_OFF,
    ASSET_ICON_LVL_UP,

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