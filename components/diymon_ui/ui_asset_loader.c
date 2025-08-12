/*
  Fichero: ./components/diymon_ui/ui_asset_loader.c
  Fecha: 13/08/2025 - 21:15
  Último cambio: Mapeados los nuevos assets a los ficheros BTN_7, 8 y 9.
  Descripción: Implementación del gestor de assets. Se añaden las llamadas para
               cargar los iconos del nuevo panel de configuración superior.
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
 * @brief Invierte el orden de los bytes para cada píxel de 16 bits (RGB565).
 *        Necesario para corregir el endianness en la plataforma.
 */
static void swap_bytes_for_rgb565(uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; i += 2) {
        uint8_t temp = data[i];
        data[i] = data[i + 1];
        data[i + 1] = temp;
    }
}

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
    
    // Corregir los bytes de los datos leídos.
    swap_bytes_for_rgb565(g_asset_buffers[id], data_size);
    
    // Apuntar el descriptor al búfer en memoria.
    g_asset_dscs[id].data = g_asset_buffers[id];

    ESP_LOGI(TAG, "Asset '%s' cargado correctamente en memoria.", filename);
    return true;
}

void ui_assets_init(void) {
    ESP_LOGI(TAG, "Precargando todos los assets de la UI...");
    // Panel de jugador
    load_asset(ASSET_ICON_EAT, "ICON_EAT.bin");
    load_asset(ASSET_ICON_GYM, "ICON_GYM.bin");
    load_asset(ASSET_ICON_ATK, "ICON_ATK.bin");
    
    // Panel de admin
    load_asset(ASSET_ICON_BRIGHTNESS, "ICON_LUZ.bin");
    load_asset(ASSET_ICON_SCREEN_OFF, "ICON_OFF.bin");
    load_asset(ASSET_ICON_ADMIN_PLACEHOLDER, "ICON_ADM.bin");

    // Panel lateral de evolución
    load_asset(ASSET_ICON_EVO_FIRE,  "LVL_FIRE.BIN");
    load_asset(ASSET_ICON_EVO_WATER, "LVL_WATE.BIN");
    load_asset(ASSET_ICON_EVO_EARTH, "LVL_EART.BIN");
    load_asset(ASSET_ICON_EVO_WIND,  "LVL_WIND.BIN");
    load_asset(ASSET_ICON_EVO_BACK,  "LVL_DOWN.BIN");

    // --- ANOTACIÓN: Carga de los nuevos iconos para el TERCER panel superior. ---
    // Asegúrate de que estos ficheros existen en /sdcard/buttons/
    load_asset(ASSET_ICON_BTN_7, "BTN_7.bin");
    load_asset(ASSET_ICON_BTN_8, "BTN_8.bin");
    load_asset(ASSET_ICON_BTN_9, "BTN_9.bin");
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