/* Fecha: 17/08/2025 - 03:09  */
/* Fichero: components/ui/ui_asset_loader.c */
/* Último cambio: Actualizada la ruta de inclusión para 'ui_assets.h' para reflejar la nueva estructura de directorios. */
/* Descripción: Implementa el cargador de assets. Se ha corregido la ruta de inclusión de los assets de imagen para que apunte a 'assets/images/ui_assets.h', adaptándose a la refactorización que centraliza todos los recursos de apariencia. */

#include "ui_asset_loader.h"
#include "assets/images/ui_assets.h" // Actualizado para reflejar la nueva ubicación
#include "esp_log.h"

static const char *TAG = "UI_ASSET_LOADER";

// --- Mapeo de IDs de assets a los punteros de los descriptores de imagen compilados ---
static const lv_img_dsc_t* g_asset_dsc_pointers[ASSET_COUNT] = {
    [ASSET_ICON_EAT]                  = &asset_btn_1,
    [ASSET_ICON_GYM]                  = &asset_btn_2,
    [ASSET_ICON_ATK]                  = &asset_btn_3,
    [ASSET_ICON_BRIGHTNESS]           = &asset_btn_4,
    [ASSET_ICON_SCREEN_OFF]           = &asset_btn_5,
    [ASSET_ICON_ADMIN_PLACEHOLDER]    = &asset_btn_6,
    [ASSET_ICON_RESET_ALL]            = &asset_btn_7,
    [ASSET_ICON_ENABLE_FILE_SERVER]   = &asset_btn_8,
    [ASSET_ICON_CONFIG_PLACEHOLDER]   = &asset_btn_9,
    [ASSET_ICON_EVO_FIRE]             = &asset_evo_1,
    [ASSET_ICON_EVO_WATER]            = &asset_evo_2,
    [ASSET_ICON_EVO_EARTH]            = &asset_evo_3,
    [ASSET_ICON_EVO_WIND]             = &asset_evo_4,
    [ASSET_ICON_EVO_BACK]             = &asset_evo_5,
};

/**
 * @brief Inicializa el gestor de assets. En este modo no hace nada.
 */
void ui_assets_init(void) {
    ESP_LOGI(TAG, "Asset loader inicializado en modo 'firmware-linked'.");
}

/**
 * @brief Desinicializa el gestor de assets. No hace nada en este modo.
 */
void ui_assets_deinit(void) {
    // No hay memoria dinámica que liberar.
}

/**
 * @brief Obtiene un puntero al descriptor de imagen de un icono compilado en el firmware.
 * @param asset_id El identificador del icono.
 * @return Un puntero constante al descriptor de imagen LVGL.
 */
const lv_img_dsc_t* ui_assets_get_icon(ui_asset_id_t asset_id) {
    if (asset_id < 0 || asset_id >= ASSET_COUNT) {
        ESP_LOGE(TAG, "Asset ID %d solicitado está fuera de rango.", asset_id);
        return NULL;
    }
    return g_asset_dsc_pointers[asset_id];
}
