/* Fecha: 16/08/2025 - 07:14  */
/* Fichero: components/diymon_ui/animation_loader.c */
/* Último cambio: Eliminados los logs de depuración hexadecimal para optimizar el tiempo de carga de las animaciones. */
/* Descripción: Se ha eliminado el log de diagnóstico que imprimía el contenido hexadecimal de los nombres de fichero, ya que causaba un retardo notable al iniciar las animaciones. Se mantiene la lógica de comparación esencial, mejorando el rendimiento. */

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
    
    ESP_LOGD(TAG, "Gestor de animación inicializado. Buffer de %u bytes.", (unsigned int)buffer_size);
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
    ESP_LOGD(TAG, "Abriendo directorio de animación (LVGL): '%s' para contar frames con prefijo '%s'", path, prefix);
    
    lv_fs_dir_t d;
    lv_fs_res_t res = lv_fs_dir_open(&d, path);

    if(res != LV_FS_RES_OK) {
        ESP_LOGE(TAG, "No se pudo abrir el directorio (LVGL): %s. Código de error: %d", path, res);
        return 0;
    }

    char fn[256];
    size_t prefix_len = strlen(prefix);

    while(lv_fs_dir_read(&d, fn, sizeof(fn)) == LV_FS_RES_OK && fn[0] != '\0') {
        // Compara si el nombre del fichero empieza con el prefijo deseado.
        if (strncmp(fn, prefix, prefix_len) == 0) {
            count++;
        }
    }

    lv_fs_dir_close(&d);
    ESP_LOGD(TAG, "Cierre de directorio. Total de fotogramas con prefijo '%s': %d", prefix, count);
    return count;
}
