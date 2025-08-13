/*
  Fichero: ./components/diymon_ui/ui_asset_loader.c
  Fecha: 13/08/2025 - 12:43 
  Último cambio: Actualizado para usar los assets compilados desde ficheros .c.
  Descripción: Se modifica la lógica para enlazar directamente con las variables de descriptores de imagen (ej: &BTN_1) en lugar de usar los símbolos de datos binarios (_binary_*). Esto alinea el código con el nuevo sistema de compilación de assets.
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
    g_asset_dscs[ASSET_ICON_ENABLE_FTP] = &BTN_8;
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
