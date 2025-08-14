/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_asset_loader.h
# Fecha: $timestamp
# Último cambio: Corregido el tipo de retorno de `ui_assets_get_icon` a `const lv_img_dsc_t*`.
# Descripción: Se corrige la declaración de `ui_assets_get_icon` para que devuelva un puntero a un descriptor de imagen (`const lv_img_dsc_t*`), coincidiendo con la nueva implementación que utiliza assets compilados en el firmware. Esto resuelve el error de compilación por tipos conflictivos.
*/
#ifndef UI_ASSET_LOADER_H
#define UI_ASSET_LOADER_H

#include "lvgl.h"

// Identificadores únicos para cada asset precargado.
typedef enum {
    // Iconos del panel de jugador (superior 1)
    ASSET_ICON_EAT,
    ASSET_ICON_GYM,
    ASSET_ICON_ATK,

    // Iconos del panel de administración (superior 2)
    ASSET_ICON_BRIGHTNESS,
    ASSET_ICON_SCREEN_OFF,
    ASSET_ICON_ADMIN_PLACEHOLDER,
    
    // Iconos del panel de configuración (superior 3)
    ASSET_ICON_RESET_ALL,
    ASSET_ICON_ENABLE_FILE_SERVER,
    ASSET_ICON_CONFIG_PLACEHOLDER,

    // Iconos del panel de evolución (lateral)
    ASSET_ICON_EVO_FIRE,
    ASSET_ICON_EVO_WATER,
    ASSET_ICON_EVO_EARTH,
    ASSET_ICON_EVO_WIND,
    ASSET_ICON_EVO_BACK,

    ASSET_COUNT // Utilidad para contar el número de assets
} ui_asset_id_t;

/**
 * @brief Inicializa el gestor de assets.
 */
void ui_assets_init(void);

/**
 * @brief Desinicializa el gestor de assets.
 */
void ui_assets_deinit(void);

/**
 * @brief Obtiene un puntero al descriptor de imagen de un icono compilado en el firmware.
 * @param asset_id El identificador del icono a obtener.
 * @return Un puntero constante al descriptor de imagen LVGL.
 */
const lv_img_dsc_t* ui_assets_get_icon(ui_asset_id_t asset_id);

#endif // UI_ASSET_LOADER_H
