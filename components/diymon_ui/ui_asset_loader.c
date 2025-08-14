/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_asset_loader.c
# Fecha: $timestamp
# Último cambio: Modificado para cargar los iconos de los botones desde archivos .bin en la SD.
# Descripción: Gestor de assets de la UI. En lugar de usar assets compilados desde ficheros .c, ahora carga dinámicamente las imágenes de los botones desde /sdcard/buttons/ en formato RGB565A8, permitiendo la personalización de skins sin recompilar.
*/
#include "ui_asset_loader.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "UI_ASSET_LOADER";

// --- Constantes de configuración de assets ---
#define ASSET_BASE_PATH "/sdcard/buttons"
#define ASSET_IMG_WIDTH 50
#define ASSET_IMG_HEIGHT 50
#define LVGL_BIN_HEADER_SIZE 12

// Array de descriptores de imagen, uno para cada asset.
static lv_img_dsc_t g_asset_dscs[ASSET_COUNT];

// Mapeo de IDs de assets a nombres de fichero.
static const char* g_asset_filenames[ASSET_COUNT] = {
    [ASSET_ICON_EAT]                  = "BTN_1.bin",
    [ASSET_ICON_GYM]                  = "BTN_2.bin",
    [ASSET_ICON_ATK]                  = "BTN_3.bin",
    [ASSET_ICON_BRIGHTNESS]           = "BTN_4.bin",
    [ASSET_ICON_SCREEN_OFF]           = "BTN_5.bin",
    [ASSET_ICON_ADMIN_PLACEHOLDER]    = "BTN_6.bin",
    [ASSET_ICON_RESET_ALL]            = "BTN_7.bin",
    [ASSET_ICON_ENABLE_FILE_SERVER]   = "BTN_8.bin",
    [ASSET_ICON_CONFIG_PLACEHOLDER]   = "BTN_9.bin",
    [ASSET_ICON_EVO_FIRE]             = "EVO_1.bin",
    [ASSET_ICON_EVO_WATER]            = "EVO_2.bin",
    [ASSET_ICON_EVO_EARTH]            = "EVO_3.bin",
    [ASSET_ICON_EVO_WIND]             = "EVO_4.bin",
    [ASSET_ICON_EVO_BACK]             = "EVO_5.bin",
};

/**
 * @brief Carga todos los iconos desde la tarjeta SD y los almacena en RAM.
 */
void ui_assets_init(void) {
    ESP_LOGI(TAG, "Cargando assets de iconos desde la tarjeta SD...");

    for (int i = 0; i < ASSET_COUNT; i++) {
        char path_buf[128];
        snprintf(path_buf, sizeof(path_buf), "%s/%s", ASSET_BASE_PATH, g_asset_filenames[i]);

        // El formato es RGB565A8, que usa 3 bytes por píxel.
        size_t buffer_size = ASSET_IMG_WIDTH * ASSET_IMG_HEIGHT * 3;
        uint8_t *img_buf = malloc(buffer_size);

        if (!img_buf) {
            ESP_LOGE(TAG, "Fallo al reservar %u bytes para %s", (unsigned int)buffer_size, g_asset_filenames[i]);
            g_asset_dscs[i].data = NULL; // Marcar como fallido
            continue;
        }

        FILE* f = fopen(path_buf, "rb");
        if (!f) {
            ESP_LOGE(TAG, "No se pudo abrir el asset: %s", path_buf);
            free(img_buf);
            g_asset_dscs[i].data = NULL;
            continue;
        }

        fseek(f, LVGL_BIN_HEADER_SIZE, SEEK_SET); // Omitir la cabecera del fichero .bin
        size_t bytes_read = fread(img_buf, 1, buffer_size, f);
        fclose(f);

        if (bytes_read != buffer_size) {
            ESP_LOGW(TAG, "Lectura incompleta para %s. Leidos %u de %u bytes.", path_buf, (unsigned int)bytes_read, (unsigned int)buffer_size);
        }

        g_asset_dscs[i].header.cf = LV_COLOR_FORMAT_RGB565A8;
        g_asset_dscs[i].header.w = ASSET_IMG_WIDTH;
        g_asset_dscs[i].header.h = ASSET_IMG_HEIGHT;
        g_asset_dscs[i].header.stride = ASSET_IMG_WIDTH * 2; // Stride para la parte de color (RGB565)
        g_asset_dscs[i].data_size = buffer_size;
        g_asset_dscs[i].data = img_buf;
    }

    ESP_LOGI(TAG, "Carga de assets desde SD finalizada.");
}

/**
 * @brief Libera la memoria de todos los iconos cargados.
 */
void ui_assets_deinit(void) {
    ESP_LOGI(TAG, "Liberando memoria de los assets de iconos.");
    for (int i = 0; i < ASSET_COUNT; i++) {
        if (g_asset_dscs[i].data) {
            free((void*)g_asset_dscs[i].data);
            g_asset_dscs[i].data = NULL;
        }
    }
}

/**
 * @brief Obtiene un puntero al descriptor de imagen de un icono ya cargado.
 */
const lv_img_dsc_t* ui_assets_get_icon(ui_asset_id_t asset_id) {
    if (asset_id < ASSET_COUNT && asset_id >= 0 && g_asset_dscs[asset_id].data != NULL) {
        return &g_asset_dscs[asset_id];
    }
    ESP_LOGE(TAG, "Asset ID %d solicitado es invalido o no se pudo cargar.", asset_id);
    return NULL; // Devuelve NULL si el asset no se pudo cargar o el ID es inválido.
}
