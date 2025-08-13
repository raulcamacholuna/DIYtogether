/*
  Fichero: ./components/diymon_ui/ui_asset_loader.c
  Fecha: 13/08/2025 - 09:37 
  Último cambio: Eliminada la función no utilizada swap_bytes_for_rgb565.
  Descripción: Implementación del gestor de assets. Se ha eliminado una función estática que no se estaba utilizando para limpiar el código y resolver una advertencia del compilador.
*/
#include "ui_asset_loader.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "UI_ASSET_LOADER";

// Almacenamiento estático para los descriptores y los datos de los iconos.
static lv_img_dsc_t g_asset_dscs[ASSET_COUNT];
static uint8_t* g_asset_buffers[ASSET_COUNT] = {NULL};

/**
 * @brief Carga un único asset (icono) desde un fichero a la memoria.
 */
static bool load_asset(ui_asset_id_t id, const char* filename) {
    char path[128];
    ui_helpers_build_asset_path(path, sizeof(path), filename);

    FILE* f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "No se pudo abrir el asset: %s", path);
        return false;
    }

    // Leer la cabecera LVGL directamente.
    fread(&g_asset_dscs[id].header, 1, sizeof(lv_image_header_t), f);

    // Calcular tamaño, reservar búfer y leer los datos de píxeles.
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    size_t data_size = file_size - sizeof(lv_image_header_t);
    g_asset_dscs[id].data_size = data_size;

    g_asset_buffers[id] = (uint8_t*)malloc(data_size);
    if (!g_asset_buffers[id]) {
        ESP_LOGE(TAG, "Fallo al reservar memoria para el asset %s", filename);
        fclose(f);
        return false;
    }
    
    fseek(f, sizeof(lv_image_header_t), SEEK_SET);
    fread(g_asset_buffers[id], 1, data_size, f);
    fclose(f);
    
    // Apuntar el descriptor al búfer en memoria.
    g_asset_dscs[id].data = g_asset_buffers[id];

    ESP_LOGI(TAG, "Asset '%s' cargado correctamente en memoria.", filename);
    return true;
}

void ui_assets_init(void) {
    ESP_LOGI(TAG, "Precargando todos los assets de la UI con la nueva estructura de nombres...");
    
    // Panel de jugador (iconos 1-3)
    load_asset(ASSET_ICON_EAT, "BTN_1.bin");
    load_asset(ASSET_ICON_GYM, "BTN_2.bin");
    load_asset(ASSET_ICON_ATK, "BTN_3.bin");
    
    // Panel de admin (iconos 4-6)
    load_asset(ASSET_ICON_BRIGHTNESS, "BTN_4.bin");
    load_asset(ASSET_ICON_SCREEN_OFF, "BTN_5.bin");
    load_asset(ASSET_ICON_ADMIN_PLACEHOLDER, "BTN_6.bin");

    // Panel de configuración (iconos 7-9)
    load_asset(ASSET_ICON_RESET_ALL, "BTN_7.bin");
    load_asset(ASSET_ICON_ENABLE_FTP, "BTN_8.bin");
    load_asset(ASSET_ICON_CONFIG_PLACEHOLDER, "BTN_9.bin");

    // Panel lateral de evolución (iconos EVO 1-5)
    load_asset(ASSET_ICON_EVO_FIRE,  "EVO_1.bin");
    load_asset(ASSET_ICON_EVO_WATER, "EVO_2.bin");
    load_asset(ASSET_ICON_EVO_EARTH, "EVO_3.bin");
    load_asset(ASSET_ICON_EVO_WIND,  "EVO_4.bin");
    load_asset(ASSET_ICON_EVO_BACK,  "EVO_5.bin");
}

void ui_assets_deinit(void) {
    for (int i = 0; i < ASSET_COUNT; i++) {
        if (g_asset_buffers[i]) {
            free(g_asset_buffers[i]);
            g_asset_buffers[i] = NULL;
        }
    }
}

const lv_img_dsc_t* ui_assets_get_icon(ui_asset_id_t asset_id) {
    if (asset_id < ASSET_COUNT && asset_id >= 0) {
        return &g_asset_dscs[asset_id];
    }
    return NULL;
}
