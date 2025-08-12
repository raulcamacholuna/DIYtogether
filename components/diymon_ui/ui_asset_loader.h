/*
  Fichero: ./components/diymon_ui/ui_asset_loader.h
  Fecha: 13/08/2025 - 21:15
  Último cambio: Añadidos los IDs de asset para el tercer panel superior.
  Descripción: Interfaz pública para el gestor de assets. Se definen los
               identificadores para todos los iconos, incluyendo el nuevo panel.
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
    ASSET_ICON_BRIGHTNESS,
    ASSET_ICON_SCREEN_OFF,
    ASSET_ICON_ADMIN_PLACEHOLDER,

    // Iconos del panel de evolución (lateral)
    ASSET_ICON_EVO_FIRE,
    ASSET_ICON_EVO_WATER,
    ASSET_ICON_EVO_EARTH,
    ASSET_ICON_EVO_WIND,
    ASSET_ICON_EVO_BACK,

    // Iconos del panel de configuración (superior 3)
    ASSET_ICON_BTN_7,
    ASSET_ICON_BTN_8,
    ASSET_ICON_BTN_9,

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