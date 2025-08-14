/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_asset_loader.c
# Fecha: `$timestamp
# Último cambio: Corregido para ser sintácticamente válido en C.
# Descripción: Gestor de assets de la UI. Enlaza los identificadores de assets con los descriptores de imagen LVGL compilados.
*/
#include "ui_asset_loader.h"
#include "images.h"
#include "esp_log.h"

static const char *TAG = "UI_ASSET_LOADER";

static const lv_img_dsc_t* g_asset_dscs[ASSET_COUNT];

void ui_assets_init(void) {
    ESP_LOGI(TAG, "Inicializando assets desde variables compiladas...");

    // Panel Jugador
    g_asset_dscs[ASSET_ICON_EAT] = &BTN_1;
    g_asset_dscs[ASSET_ICON_GYM] = &BTN_2;
    g_asset_dscs[ASSET_ICON_ATK] = &BTN_3;
    
    // Panel Admin
    g_asset_dscs[ASSET_ICON_BRIGHTNESS] = &BTN_4;
    g_asset_dscs[ASSET_ICON_SCREEN_OFF] = &BTN_5;
    g_asset_dscs[ASSET_ICON_ADMIN_PLACEHOLDER] = &BTN_6;

    // Panel Config
    g_asset_dscs[ASSET_ICON_RESET_ALL] = &BTN_7;
    g_asset_dscs[ASSET_ICON_ENABLE_FILE_SERVER] = &BTN_8;
    g_asset_dscs[ASSET_ICON_CONFIG_PLACEHOLDER] = &BTN_9;

    // Panel Evolución
    g_asset_dscs[ASSET_ICON_EVO_FIRE]  = &EVO_1;
    g_asset_dscs[ASSET_ICON_EVO_WATER] = &EVO_2;
    g_asset_dscs[ASSET_ICON_EVO_EARTH] = &EVO_3;
    g_asset_dscs[ASSET_ICON_EVO_WIND]  = &EVO_4;
    g_asset_dscs[ASSET_ICON_EVO_BACK]  = &EVO_5;

    ESP_LOGI(TAG, "Todos los assets han sido enlazados.");
}

void ui_assets_deinit(void) {
    // No es necesario hacer nada aquí ya que los assets están en la sección .rodata
}

const lv_img_dsc_t* ui_assets_get_icon(ui_asset_id_t asset_id) {
    if (asset_id < ASSET_COUNT && asset_id >= 0) {
        return g_asset_dscs[asset_id];
    }
    return NULL;
}
