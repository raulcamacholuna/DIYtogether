/*
 * Fichero: ./components/diymon_ui/ui_action_animations.c
 * Fecha: 13/08/2025 - 06:21 
 * Último cambio: Desplazada la animación 30 píxeles hacia arriba.
 * Descripción: Se ha modificado el alineamiento del objeto de imagen de la animación para desplazarlo 30 píxeles hacia arriba, mejorando su posición en la pantalla.
 */
#include "ui_action_animations.h"
#include "animation_loader.h"
#include "diymon_ui_helpers.h"
#include "ui_idle_animation.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "UI_ACTION_ANIM";

// --- Variables Globales y Estáticas ---
lv_obj_t *g_animation_img_obj;
static animation_t s_animation_player; // Player local para esta animación

static lv_timer_t *s_anim_timer;
static bool s_is_action_in_progress = false;
static int s_current_frame_index;

#define FRAME_INTERVAL_MS 500

// --- Declaraciones de Funciones Internas ---
static void animation_timer_cb(lv_timer_t *timer);
static void animation_finished(void);
static const char* get_anim_prefix(diymon_action_id_t action_id);

// --- Implementación de Funciones Públicas ---

void ui_action_animations_create(lv_obj_t *parent) {
    // Solo crea el objeto de imagen. El buffer se gestionará por animación.
    g_animation_img_obj = lv_image_create(parent);
    lv_obj_set_style_bg_opa(g_animation_img_obj, LV_OPA_TRANSP, 0);
    lv_obj_align(g_animation_img_obj, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void ui_action_animations_play(diymon_action_id_t action_id) {
    if (s_is_action_in_progress || action_id >= ACTION_ID_COUNT) return;

    s_is_action_in_progress = true;
    
    ui_idle_animation_pause();

    const char *prefix = get_anim_prefix(action_id);
    char path_buffer[128];
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "");
    size_t len = strlen(path_buffer);
    if (len > 0 && path_buffer[len - 1] == '/') path_buffer[len - 1] = '\0';
    
    uint16_t frame_count = animation_loader_count_frames(path_buffer, prefix);
    if (frame_count == 0) {
        ESP_LOGE(TAG, "No se encontraron fotogramas para la animación '%s' en '%s'.", prefix, path_buffer);
        animation_finished(); 
        return;
    }
    
    ESP_LOGI(TAG, "Reproduciendo animación '%s' (%d fotogramas) a %dms/frame.", prefix, frame_count, FRAME_INTERVAL_MS);

    s_animation_player = animation_loader_init(path_buffer, 150, 230, frame_count);
    if(s_animation_player.img_dsc.data == NULL) {
        ESP_LOGE(TAG, "No se pudo crear el reproductor para la animación de acción.");
        animation_finished();
        return;
    }
    
    lv_image_set_src(g_animation_img_obj, &s_animation_player.img_dsc);

    s_current_frame_index = 0;
    if (animation_loader_load_frame(&s_animation_player, s_current_frame_index, prefix)) {
        lv_obj_invalidate(g_animation_img_obj);
        s_anim_timer = lv_timer_create(animation_timer_cb, FRAME_INTERVAL_MS, (void*)(intptr_t)action_id);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el primer fotograma (%s).", prefix);
        animation_finished();
    }
}

void ui_action_animations_destroy(void) {
    // La limpieza se hace en animation_finished al terminar cada animación
}

static void animation_timer_cb(lv_timer_t *timer) {
    s_current_frame_index++;
    if (s_current_frame_index >= s_animation_player.frame_count) {
        animation_finished();
        return;
    }

    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)timer->user_data;
    const char *prefix = get_anim_prefix(action_id);

    if (animation_loader_load_frame(&s_animation_player, s_current_frame_index, prefix)) {
        lv_obj_invalidate(g_animation_img_obj);
    } else {
        ESP_LOGW(TAG, "No se pudo cargar el fotograma %d para %s. Finalizando animación.", s_current_frame_index + 1, prefix);
        animation_finished();
    }
}

static void animation_finished(void) {
    if (s_anim_timer) {
        lv_timer_del(s_anim_timer);
        s_anim_timer = NULL;
    }
    
    animation_loader_free(&s_animation_player);
    
    ui_idle_animation_resume();
    
    s_is_action_in_progress = false;
    ESP_LOGI(TAG, "Animación de acción finalizada. Control devuelto a idle.");
}

static const char* get_anim_prefix(diymon_action_id_t action_id) {
    switch(action_id) {
        case ACTION_ID_COMER:     return "EAT_";
        case ACTION_ID_EJERCICIO: return "GYM_";
        case ACTION_ID_ATACAR:    return "ATK_";
        default:                  return "";
    }
}
