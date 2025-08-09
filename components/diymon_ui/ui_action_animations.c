/*
 * Fichero: ./components/diymon_ui/ui_action_animations.c
 * Fecha: 10/08/2025 - 03:15
 * Último cambio: Creación del fichero.
 * Descripción: Implementación del módulo de animaciones de acción. Centraliza la creación y reproducción de las imágenes que se muestran al pulsar los botones.
 */
#include "ui_action_animations.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "UI_ACTION_ANIM";

// Variables estáticas para los objetos gestionados por este módulo
static lv_obj_t *s_eat_anim, *s_gym_anim, *s_atk_anim;
static bool s_is_action_in_progress = false;

typedef struct {
    lv_obj_t **action_obj_ptr;
    uint32_t duration_ms;
} action_def_t;

// Catálogo que asocia un ID de acción con su objeto de imagen y duración
static const action_def_t ACTION_CATALOG[] = {
    [ACTION_ID_COMER]     = { &s_eat_anim, 2500 },
    [ACTION_ID_EJERCICIO] = { &s_gym_anim, 4000 },
    [ACTION_ID_ATACAR]    = { &s_atk_anim, 3000 },
};

void ui_action_animations_create(lv_obj_t *parent) {
    char path_buffer[128];
    char lvgl_path[132];

    s_eat_anim = lv_img_create(parent);
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "manzana.png");
    snprintf(lvgl_path, sizeof(lvgl_path), "S:%s", path_buffer);
    lv_img_set_src(s_eat_anim, lvgl_path);
    lv_obj_align(s_eat_anim, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_eat_anim, LV_OBJ_FLAG_HIDDEN);

    s_gym_anim = lv_img_create(parent);
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "pesas.png");
    snprintf(lvgl_path, sizeof(lvgl_path), "S:%s", path_buffer);
    lv_img_set_src(s_gym_anim, lvgl_path);
    lv_obj_align(s_gym_anim, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_gym_anim, LV_OBJ_FLAG_HIDDEN);

    s_atk_anim = lv_img_create(parent);
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ataque.png");
    snprintf(lvgl_path, sizeof(lvgl_path), "S:%s", path_buffer);
    lv_img_set_src(s_atk_anim, lvgl_path);
    lv_obj_align(s_atk_anim, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_atk_anim, LV_OBJ_FLAG_HIDDEN);
    
    ESP_LOGI(TAG, "Objetos de animación de acciones creados.");
}

static void action_finished_callback(lv_timer_t *timer) {
    lv_obj_t *action_obj = (lv_obj_t *)timer->user_data;
    lv_obj_add_flag(action_obj, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_t *idle_obj = (lv_obj_t*)lv_timer_get_user_data(timer);
    if(idle_obj) {
       lv_obj_clear_flag(idle_obj, LV_OBJ_FLAG_HIDDEN);
    }
    
    s_is_action_in_progress = false;
    lv_timer_del(timer);
}

void ui_action_animations_play(diymon_action_id_t action_id, lv_obj_t* idle_obj) {
    if (s_is_action_in_progress || action_id >= ACTION_ID_COUNT) {
        return;
    }
    
    const action_def_t *action_def = &ACTION_CATALOG[action_id];
    if (!action_def || !action_def->action_obj_ptr || !(*action_def->action_obj_ptr)) {
        return;
    }
    
    ESP_LOGI(TAG, "Iniciando animación para acción ID: %d", action_id);
    s_is_action_in_progress = true;
    
    if (idle_obj) {
        lv_obj_add_flag(idle_obj, LV_OBJ_FLAG_HIDDEN);
    }
    
    lv_obj_clear_flag(*action_def->action_obj_ptr, LV_OBJ_FLAG_HIDDEN);
    
    lv_timer_create(action_finished_callback, action_def->duration_ms, idle_obj);
}