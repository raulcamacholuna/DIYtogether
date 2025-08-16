/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_asset_loader.c
# Fecha: $timestamp
# Último cambio: Revertido a un modelo de assets enlazados en firmware para solucionar el lag de la SD.
# Descripción: Se ha revertido la estrategia de carga de assets. En lugar de generar rutas de fichero para la SD, ahora se devuelve un puntero a un descriptor de imagen (`lv_img_dsc_t`) que estará compilado directamente en el firmware. Esto elimina las lentas operaciones de E/S de la tarjeta SD durante el renderizado, solucionando los graves problemas de rendimiento ("lag").
*/
#include "ui_asset_loader.h"
#include "ui_assets.h" // Este fichero contendrá las declaraciones 'extern' de los assets
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
