/*
 * Fichero: ./components/diymon_ui/ui_action_animations.c
 * Fecha: 11/08/2025 - 11:30
 * Último cambio: Implementado el conteo dinámico de fotogramas y el intervalo fijo.
 * Descripción: Se elimina el número de fotogramas y la duración fijos. Ahora,
 *              el módulo cuenta los fotogramas de animación disponibles en la SD
 *              y reproduce la animación con un intervalo fijo por fotograma,
 *              haciendo el sistema flexible y la velocidad de reproducción consistente.
 */
#include "ui_action_animations.h"
#include "animation_loader.h"
#include "diymon_ui_helpers.h"
#include "ui_idle_animation.h" 
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "UI_ACTION_ANIM";

// --- Variables de Estado Estáticas ---
static lv_obj_t *s_anim_img_obj;
static lv_timer_t *s_anim_timer;
static animation_t g_animation_player;
static bool s_is_action_in_progress = false;
static int s_current_frame_index;
static lv_obj_t *s_idle_obj_ref;

// Define la velocidad de la animación. 500ms por frame
#define FRAME_INTERVAL_MS 500

static void animation_timer_cb(lv_timer_t *timer);
static void animation_finished(void);
static const char* get_anim_prefix(diymon_action_id_t action_id);

void ui_action_animations_create(lv_obj_t *parent) {
    s_anim_img_obj = lv_img_create(parent);
    lv_obj_align(s_anim_img_obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_anim_img_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(s_anim_img_obj);

    // El número de frames ya no se conoce en la inicialización.
    g_animation_player = animation_loader_init(NULL, 170, 320, 0);
    if (g_animation_player.img_dsc.data == NULL) {
        ESP_LOGE(TAG, "FALLO CRÍTICO: No se pudo reservar memoria para el búfer de animación compartido.");
    } else {
        ESP_LOGI(TAG, "Búfer de animación compartido pre-reservado correctamente.");
    }
}

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
    
    char path_buffer[128];
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "");
    size_t len = strlen(path_buffer);
    if (len > 0 && path_buffer[len - 1] == '/') path_buffer[len - 1] = '\0';
    
    // --- LÓGICA DINÁMICA ---
    // 1. Contar cuántos fotogramas existen para esta animación.
    uint16_t frame_count = animation_loader_count_frames(path_buffer, prefix);
    if (frame_count == 0) {
        ESP_LOGE(TAG, "No se encontraron fotogramas para la animación '%s' en '%s'. Abortando.", prefix, path_buffer);
        s_is_action_in_progress = false;
        if(s_idle_obj_ref) ui_idle_animation_resume();
        return;
    }
    
    ESP_LOGI(TAG, "Reproduciendo animación '%s' (%d fotogramas) a %dms/frame.", prefix, frame_count, FRAME_INTERVAL_MS);

    if (s_idle_obj_ref) {
        ui_idle_animation_pause();
        lv_obj_add_flag(s_idle_obj_ref, LV_OBJ_FLAG_HIDDEN);
    }
    
    if (g_animation_player.base_path) free(g_animation_player.base_path);
    g_animation_player.base_path = strdup(path_buffer);
    g_animation_player.frame_count = frame_count; // Actualizar el contador de frames.

    lv_img_set_src(s_anim_img_obj, &g_animation_player.img_dsc);
    
    s_current_frame_index = 0;
    if (animation_loader_load_frame(&g_animation_player, s_current_frame_index, prefix)) {
        lv_obj_clear_flag(s_anim_img_obj, LV_OBJ_FLAG_HIDDEN);
        // 2. Crear el temporizador con el intervalo fijo.
        s_anim_timer = lv_timer_create(animation_timer_cb, FRAME_INTERVAL_MS, (void*)(intptr_t)action_id);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el primer fotograma (%s). Abortando animación.", prefix);
        animation_finished();
    }
}

static void animation_timer_cb(lv_timer_t *timer) {
    s_current_frame_index++;
    // La condición ahora usa el contador de frames dinámico.
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
    
    if (s_idle_obj_ref) {
        lv_obj_clear_flag(s_idle_obj_ref, LV_OBJ_FLAG_HIDDEN);
        ui_idle_animation_resume();
    }
    
    if (g_animation_player.base_path) {
        free(g_animation_player.base_path);
        g_animation_player.base_path = NULL;
    }
    g_animation_player.frame_count = 0;

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