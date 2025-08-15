/* Fecha: 15/08/2025 - 05:27  */
/* Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\animation_loader.c */
/* Último cambio: Añadido un log de depuración hexadecimal para la comparación de nombres de fichero. */
/* Descripción: Se ha añadido un log de diagnóstico avanzado en nimation_loader_count_frames. Ahora, para cada fichero leído del directorio, se imprimirá su nombre en formato de texto y en hexadecimal, y se hará lo mismo con el prefijo buscado. Esto permitirá una comparación byte a byte para identificar la causa raíz del fallo en strncmp, ya sea por nombres 8.3, caracteres ocultos u otros problemas. */

#include "animation_loader.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "ANIM_LOADER";
#define LVGL_BIN_HEADER_SIZE 12

animation_t animation_loader_init(const char *path, uint16_t width, uint16_t height, uint16_t num_frames) {
    animation_t anim = { 0 };
    anim.base_path = path ? strdup(path) : NULL;
    anim.frame_count = num_frames;
    anim.width = width;
    anim.height = height;
    
    uint32_t rgb_stride = width * 2; 
    size_t buffer_size = (size_t)width * height * 3;

    anim.img_dsc.data = (uint8_t *)malloc(buffer_size);
    if (!anim.img_dsc.data) { 
        ESP_LOGE(TAG, "Fallo al reservar buffer de animación de tamaño %u!", (unsigned int)buffer_size);
        animation_loader_free(&anim); 
        return anim; 
    }
    
    anim.img_dsc.header.w = width;
    anim.img_dsc.header.h = height;
    anim.img_dsc.header.stride = rgb_stride;
    anim.img_dsc.header.cf = LV_COLOR_FORMAT_RGB565A8;
    anim.img_dsc.data_size = buffer_size;
    
    ESP_LOGI(TAG, "Gestor de animación inicializado. Buffer de %u bytes.", (unsigned int)buffer_size);
    return anim;
}

bool animation_loader_load_frame(animation_t *anim, uint16_t frame_index, const char *prefix) {
    if (!anim || !anim->base_path || !anim->img_dsc.data) return false;
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "%s/%s%d.bin", anim->base_path, prefix, frame_index + 1);

    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, full_path, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        ESP_LOGW(TAG, "Fallo al abrir frame (LVGL): %s", full_path);
        return false;
    }
    
    lv_fs_seek(&f, LVGL_BIN_HEADER_SIZE, LV_FS_SEEK_SET);
    uint32_t bytes_read = 0;
    lv_fs_read(&f, (void *)anim->img_dsc.data, anim->img_dsc.data_size, &bytes_read);
    lv_fs_close(&f);

    return true;
}

void animation_loader_free(animation_t *anim) {
    if (!anim) return;
    if(anim->base_path) {
        free(anim->base_path);
        anim->base_path = NULL;
    }
    if (anim->img_dsc.data) {
        free((void*)anim->img_dsc.data);
        anim->img_dsc.data = NULL;
    }
    anim->frame_count = 0;
}

uint16_t animation_loader_count_frames(const char *path, const char *prefix) {
    if (!path || !prefix) {
        return 0;
    }

    uint16_t count = 0;
    ESP_LOGI(TAG, "Intentando abrir el directorio de animación (LVGL): '%s'", path);
    
    lv_fs_dir_t d;
    lv_fs_res_t res = lv_fs_dir_open(&d, path);

    if(res != LV_FS_RES_OK) {
        ESP_LOGE(TAG, "No se pudo abrir el directorio (LVGL): %s. Código de error: %d", path, res);
        return 0;
    }

    ESP_LOGI(TAG, "Directorio '%s' abierto. Leyendo y comparando entradas con prefijo '%s'...", path, prefix);
    char fn[256];
    size_t prefix_len = strlen(prefix);

    while(lv_fs_dir_read(&d, fn, sizeof(fn)) == LV_FS_RES_OK && fn[0] != '\0') {
        // --- INICIO DE LOG DE DIAGNÓSTICO AVANZADO ---
        ESP_LOGI(TAG, "Candidato: '%s'", fn);
        
        char hex_buf[128] = {0};
        size_t name_len = strlen(fn);
        for(int i = 0; i < name_len && i < 32; i++) {
            sprintf(hex_buf + strlen(hex_buf), "%02X ", (unsigned char)fn[i]);
        }
        ESP_LOGI(TAG, "  -> HEX Fichero: %s", hex_buf);

        memset(hex_buf, 0, sizeof(hex_buf));
        for(int i = 0; i < prefix_len && i < 32; i++) {
            sprintf(hex_buf + strlen(hex_buf), "%02X ", (unsigned char)prefix[i]);
        }
        ESP_LOGI(TAG, "  -> HEX Prefijo: %s", hex_buf);

        if (strncmp(fn, prefix, prefix_len) == 0) {
            ESP_LOGI(TAG, "  -> ¡COINCIDENCIA!");
            count++;
        }
        // --- FIN DE LOG DE DIAGNÓSTICO AVANZADO ---
    }

    lv_fs_dir_close(&d);
    ESP_LOGI(TAG, "Cierre de directorio. Total de fotogramas con prefijo '%s': %d", prefix, count);
    return count;
}
