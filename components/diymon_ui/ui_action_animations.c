/*
 * Fichero: ./components/diymon_ui/ui_action_animations.c
 * Fecha: 10/08/2025 - 21:00
 * Último cambio: Implementado el patrón de búfer de animación compartido.
 * Descripción: Este módulo ahora gestiona el único búfer de animación para toda la UI.
 *              Expone una función `ui_action_animations_get_player` para que otros
 *              módulos (como `ui_idle_animation`) puedan usar este búfer compartido,
 *              solucionando el error de `Load access fault` por falta de memoria.
 */
#include "ui_action_animations.h"
#include "animation_loader.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "UI_ACTION_ANIM";

// --- Variables de Estado Estáticas ---
static lv_obj_t *s_anim_img_obj;
static lv_timer_t *s_anim_timer;
static animation_t g_animation_player; // Renombrado a 'g_' para indicar que es accesible
static bool s_is_action_in_progress = false;
static int s_current_frame_index;
static lv_obj_t *s_idle_obj_ref;

#define ACTION_FRAME_COUNT 4
#define ACTION_ANIM_DURATION 750
#define FRAME_INTERVAL (ACTION_ANIM_DURATION / ACTION_FRAME_COUNT)

static void animation_timer_cb(lv_timer_t *timer);
static void animation_finished(void);
static const char* get_anim_prefix(diymon_action_id_t action_id);

void ui_action_animations_create(lv_obj_t *parent) {
    s_anim_img_obj = lv_img_create(parent);
    lv_obj_align(s_anim_img_obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_anim_img_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(s_anim_img_obj);

    g_animation_player = animation_loader_init(NULL, 170, 320, ACTION_FRAME_COUNT);
    if (g_animation_player.img_dsc.data == NULL) {
        ESP_LOGE(TAG, "FALLO CRÍTICO: No se pudo reservar memoria para el búfer de animación compartido.");
    } else {
        ESP_LOGI(TAG, "Búfer de animación compartido pre-reservado correctamente.");
    }
}

// --- SOLUCIÓN: Getter para el reproductor de animación compartido ---
animation_t* ui_action_animations_get_player(void) {
    return &g_animation_player;
}

void ui_action_animations_play(diymon_action_id_t action_id, lv_obj_t* idle_obj) {
    if (s_is_action_in_progress || action_id >= ACTION_ID_COUNT) return;
    if (g_animation_player.img_dsc.data == NULL) {
        ESP_LOGE(TAG, "No se puede iniciar la animación: el búfer compartido no está disponible.");
        return;
    }

    s_is_action_in_progress = true;
    s_idle_obj_ref = idle_obj;
    const char *prefix = get_anim_prefix(action_id);
    ESP_LOGI(TAG, "Reproduciendo animación de acción: %s", prefix);

    if (s_idle_obj_ref) lv_obj_add_flag(s_idle_obj_ref, LV_OBJ_FLAG_HIDDEN);

    char path_buffer[128];
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "");
    size_t len = strlen(path_buffer);
    if (len > 0 && path_buffer[len - 1] == '/') path_buffer[len - 1] = '\0';
    
    if (g_animation_player.base_path) free(g_animation_player.base_path);
    g_animation_player.base_path = strdup(path_buffer);

    lv_img_set_src(s_anim_img_obj, &g_animation_player.img_dsc);
    
    s_current_frame_index = 0;
    if (animation_loader_load_frame(&g_animation_player, s_current_frame_index, prefix)) {
        lv_obj_clear_flag(s_anim_img_obj, LV_OBJ_FLAG_HIDDEN);
        s_anim_timer = lv_timer_create(animation_timer_cb, FRAME_INTERVAL, (void*)(intptr_t)action_id);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el primer fotograma (%s). Abortando animación.", prefix);
        animation_finished();
    }
}

static void animation_timer_cb(lv_timer_t *timer) {
    s_current_frame_index++;
    if (s_current_frame_index >= g_animation_player.frame_count) {
        animation_finished();
        return;
    }

    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)timer->user_data;
    const char *prefix = get_anim_prefix(action_id);
    if (animation_loader_load_frame(&g_animation_player, s_current_frame_index, prefix)) {
        lv_obj_invalidate(s_anim_img_obj);
    } else {
        ESP_LOGW(TAG, "No se pudo cargar el fotograma %d para %s. Finalizando.", s_current_frame_index + 1, prefix);
        animation_finished();
    }
}

static void animation_finished(void) {
    if (s_anim_img_obj) lv_obj_add_flag(s_anim_img_obj, LV_OBJ_FLAG_HIDDEN);
    if (s_idle_obj_ref) lv_obj_clear_flag(s_idle_obj_ref, LV_OBJ_FLAG_HIDDEN);
    
    if (g_animation_player.base_path) {
        free(g_animation_player.base_path);
        g_animation_player.base_path = NULL;
    }

    if (s_anim_timer) {
        lv_timer_del(s_anim_timer);
        s_anim_timer = NULL;
    }
    ESP_LOGI(TAG, "Animación finalizada. Control devuelto.");
    s_is_action_in_progress = false;
}

void ui_action_animations_destroy(void) {
    ESP_LOGI(TAG, "Liberando búfer de animación de acción compartido.");
    animation_loader_free(&g_animation_player);
}

static const char* get_anim_prefix(diymon_action_id_t action_id) {
    switch(action_id) {
        case ACTION_ID_COMER:     return "EAT_";
        case ACTION_ID_EJERCICIO: return "GYM_";
        case ACTION_ID_ATACAR:    return "ATK_";
        default:                  return "";
    }
}