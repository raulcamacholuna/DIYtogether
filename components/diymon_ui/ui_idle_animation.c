/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.c
 * Fecha: 10/08/2025 - 03:15
 * Último cambio: Creación del fichero.
 * Descripción: Implementación del módulo de animación de idle. Encapsula toda la lógica de carga, temporización y visualización de los fotogramas a pantalla completa.
 */
#include "ui_idle_animation.h"
#include "animation_loader.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "UI_IDLE_ANIM";

#define IDLE_ANIM_WIDTH      170
#define IDLE_ANIM_HEIGHT     320
#define IDLE_ANIM_FRAME_COUNT 3

static lv_obj_t *g_anim_img_obj;
static lv_timer_t *g_anim_timer;
static animation_t g_idle_animation;
static int g_current_frame_index = -1;

static int get_id_from_evo_code(const char* code_str) {
    char numeric_str[16];
    int j = 0;
    for (int i = 0; code_str[i] != '\0' && j < sizeof(numeric_str) - 1; i++) {
        if (code_str[i] >= '0' && code_str[i] <= '9') {
            numeric_str[j++] = code_str[i];
        }
    }
    numeric_str[j] = '\0';
    return atoi(numeric_str);
}

static void idle_animation_timer_cb(lv_timer_t *timer) {
    if (g_idle_animation.frame_count == 0) return;
    
    g_current_frame_index = (g_current_frame_index + 1) % g_idle_animation.frame_count;
    
    if (animation_loader_load_frame(&g_idle_animation, g_current_frame_index, "idle")) {
        lv_obj_invalidate(g_anim_img_obj);
    }
}

lv_obj_t* ui_idle_animation_start(lv_obj_t *parent) {
    const char* current_code = diymon_get_current_code();
    int evolution_id = get_id_from_evo_code(current_code);
    
    char anim_path[128];
    snprintf(anim_path, sizeof(anim_path), "/sdcard/DIYMON/%d/idle", evolution_id);

    g_idle_animation = animation_loader_init(anim_path, IDLE_ANIM_WIDTH, IDLE_ANIM_HEIGHT, IDLE_ANIM_FRAME_COUNT);

    if (g_idle_animation.frame_count > 0) {
        g_anim_img_obj = lv_img_create(parent);
        lv_img_set_src(g_anim_img_obj, &g_idle_animation.img_dsc);
        lv_obj_align(g_anim_img_obj, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_move_background(g_anim_img_obj);

        g_anim_timer = lv_timer_create(idle_animation_timer_cb, 750, NULL);
        lv_timer_ready(g_anim_timer);
        
        ESP_LOGI(TAG, "Animación de Idle iniciada desde %s.", anim_path);
        return g_anim_img_obj;
    } else {
        ESP_LOGE(TAG, "Fallo al iniciar animación de Idle desde %s.", anim_path);
        return NULL;
    }
}

void ui_idle_animation_stop(void) {
    ESP_LOGI(TAG, "Deteniendo y limpiando animación de idle.");
    if (g_anim_timer) {
        lv_timer_del(g_anim_timer);
        g_anim_timer = NULL;
    }
    // El objeto g_anim_img_obj no se borra aquí, LVGL lo hará al borrar la pantalla padre.
    animation_loader_free(&g_idle_animation);
    g_current_frame_index = -1;
}