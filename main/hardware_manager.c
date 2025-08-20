/* Fichero: main/hardware_manager.c */
/* Último cambio: Aumentado el tamaño de la pila (stack) de la tarea de LVGL para resolver el crash 'Load access fault' al tocar la pantalla. */
/* Descripción: Diagnóstico de Causa Raíz: El 'Guru Meditation Error (Load access fault)' que ocurre al interactuar con la pantalla es un síntoma clásico de desbordamiento de pila (stack overflow). La tarea de LVGL, al procesar un evento de entrada (toque), realiza llamadas a funciones complejas que consumen más memoria de pila de la que se había asignado (4KB). Esto corrompe la memoria adyacente, llevando a un fallo de acceso a memoria cuando se intenta leer una dirección inválida. El código de ejemplo del fabricante para esta placa asigna una pila de 10KB, confirmando que el valor por defecto es insuficiente.
Solución Definitiva: Se ha aumentado el tamaño de la pila para la tarea de LVGL de 4KB a 10KB, coincidiendo con el valor del ejemplo funcional del fabricante. Esto proporciona el espacio de memoria necesario para que la tarea maneje los eventos de entrada y las actualizaciones de la UI sin desbordarse, eliminando la corrupción de memoria y resolviendo el crash de forma definitiva. */
/* Último cambio: 20/08/2025 - 06:32 */
#include "hardware_manager.h"
#include "esp_log.h"
#include "bsp_api.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "sdkconfig.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

static const char *TAG = "HW_MANAGER";
#define BASE_PATH "/sdcard"

// --- Implementación del driver de LVGL v9 usando el VFS de ESP-IDF ---

static void * fs_open_cb(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode) {
    char vfs_path[256];
    if (path[0] == '/') {
        snprintf(vfs_path, sizeof(vfs_path), "%s%s", BASE_PATH, path);
    } else {
        snprintf(vfs_path, sizeof(vfs_path), "%s/%s", BASE_PATH, path);
    }

    const char * fmode_str;
    if(mode == LV_FS_MODE_WR) fmode_str = "wb";
    else if(mode == LV_FS_MODE_RD) fmode_str = "rb";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) fmode_str = "rb+";
    else return NULL;

    return fopen(vfs_path, fmode_str);
}

static lv_fs_res_t fs_close_cb(lv_fs_drv_t * drv, void * file_p) {
    fclose((FILE *)file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read_cb(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br) {
    *br = fread(buf, 1, btr, (FILE *)file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_write_cb(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw) {
    *bw = fwrite(buf, 1, btw, (FILE *)file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek_cb(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence) {
    fseek((FILE *)file_p, pos, SEEK_SET);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell_cb(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p) {
    *pos_p = ftell((FILE *)file_p);
    return LV_FS_RES_OK;
}

static void * fs_dir_open_cb(lv_fs_drv_t * drv, const char *path) {
    char vfs_path[256];
    if (path[0] == '/') {
        snprintf(vfs_path, sizeof(vfs_path), "%s%s", BASE_PATH, path);
    } else {
        snprintf(vfs_path, sizeof(vfs_path), "%s/%s", BASE_PATH, path);
    }
    return opendir(vfs_path);
}

static lv_fs_res_t fs_dir_read_cb(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len) {
    struct dirent *ent = readdir((DIR *)rddir_p);
    if(ent == NULL) {
        fn[0] = '\0';
        return LV_FS_RES_OK;
    }
    
    if(ent->d_type == DT_DIR) {
        snprintf(fn, fn_len, "/%s", ent->d_name);
    } else {
        snprintf(fn, fn_len, "%s", ent->d_name);
    }
    
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_dir_close_cb(lv_fs_drv_t * drv, void * rddir_p) {
    closedir((DIR *)rddir_p);
    return LV_FS_RES_OK;
}

esp_err_t hardware_manager_init(void) {
    ESP_LOGI(TAG, "Initializing BSP...");
    bsp_init(); 

    ESP_LOGI(TAG, "Initializing LVGL port...");
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        // [CORRECCIÓN] Aumentado el tamaño de la pila a 10KB para evitar stack overflow.
        .task_stack = 10240,
        .task_affinity = -1,
        .timer_period_ms = 5,
        .task_max_sleep_ms = 500
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));
    
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = bsp_get_panel_io_handle(),
        .panel_handle = bsp_get_display_handle(),
        .buffer_size = bsp_get_display_buffer_size(),
        .double_buffer = 1,
        .hres = bsp_get_display_hres(),
        .vres = bsp_get_display_vres(),
        .flags = { .swap_bytes = true, }
    };
    lv_disp_t * disp = lvgl_port_add_disp(&disp_cfg);

    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    ESP_LOGI(TAG, "Configuring touch driver orientation to match display rotation...");
    esp_lcd_touch_handle_t touch_handle = bsp_get_touch_handle();
    
#if defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6) || defined(CONFIG_DIYTOGETHER_BOARD_WAVESHARE_C6_147)
    ESP_ERROR_CHECK(esp_lcd_touch_set_swap_xy(touch_handle, false));
    ESP_ERROR_CHECK(esp_lcd_touch_set_mirror_x(touch_handle, false));
    ESP_ERROR_CHECK(esp_lcd_touch_set_mirror_y(touch_handle, false));
#endif

    const lvgl_port_touch_cfg_t touch_cfg = { .disp = disp, .handle = touch_handle };
    lvgl_port_add_touch(&touch_cfg);
    
    return ESP_OK;
}

void hardware_manager_mount_lvgl_filesystem(void)
{
    ESP_LOGI(TAG, "Registrando el sistema de ficheros VFS (/sdcard) con LVGL...");
    
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    fs_drv.letter = 'S';
    fs_drv.open_cb = fs_open_cb;
    fs_drv.close_cb = fs_close_cb;
    fs_drv.read_cb = fs_read_cb;
    fs_drv.write_cb = fs_write_cb;
    fs_drv.seek_cb = fs_seek_cb;
    fs_drv.tell_cb = fs_tell_cb;
    fs_drv.dir_open_cb = fs_dir_open_cb;
    fs_drv.dir_read_cb = fs_dir_read_cb;
    fs_drv.dir_close_cb = fs_dir_close_cb;

    lv_fs_drv_register(&fs_drv);
    
    ESP_LOGI(TAG, "Sistema de ficheros montado para LVGL en la unidad 'S:' (apuntando a %s)", BASE_PATH);
}
