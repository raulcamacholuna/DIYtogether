/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.c
 * Fecha: 13/08/2025 - 05:59 
 * Último cambio: Adaptado a la gestión de memoria sin buffer compartido.
 * Descripción: La animación de reposo ahora crea y gestiona su propio nimation_t player, cargando los fotogramas desde la SD. Ya no depende de un búfer compartido, lo que simplifica la lógica y previene conflictos de memoria.
 */
#include "ui_idle_animation.h"
#include "ui_action_animations.h" 
#include "animation_loader.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "UI_IDLE_ANIM";

#define IDLE_ANIM_FRAME_COUNT 3
#define IDLE_FRAME_INTERVAL 1500

static lv_timer_t *g_anim_timer;
static animation_t s_idle_animation_player;
static int g_current_frame_index = -1;
static bool g_is_idle_running = false;

static void idle_animation_timer_cb(lv_timer_t *timer) {
    if (!g_is_idle_running || s_idle_animation_player.frame_count == 0) return;
    
    g_current_frame_index = (g_current_frame_index + 1) % s_idle_animation_player.frame_count;
    
    if (animation_loader_load_frame(&s_idle_animation_player, g_current_frame_index, "ANIM_IDLE_")) {
        if (g_animation_img_obj) {
            lv_img_set_src(g_animation_img_obj, &s_idle_animation_player.img_dsc);
            lv_obj_invalidate(g_animation_img_obj);
        }
    }
}

lv_obj_t* ui_idle_animation_start(lv_obj_t *parent) {
    char anim_path[128];
    ui_helpers_build_asset_path(anim_path, sizeof(anim_path), "");
    size_t len = strlen(anim_path);
    if (len > 0 && anim_path[len - 1] == '/') anim_path[len - 1] = '\0';
    
    s_idle_animation_player = animation_loader_init(anim_path, 150, 230, IDLE_ANIM_FRAME_COUNT);
    if (s_idle_animation_player.img_dsc.data == NULL) {
        ESP_LOGE(TAG, "No se pudo crear el reproductor para la animación de idle.");
        return NULL;
    }

    g_is_idle_running = true;
    g_anim_timer = lv_timer_create(idle_animation_timer_cb, IDLE_FRAME_INTERVAL, NULL);
    lv_timer_ready(g_anim_timer);
    
    ESP_LOGI(TAG, "Animación de Idle iniciada desde %s.", anim_path);
    return g_animation_img_obj;
}

void ui_idle_animation_stop(void) {
    ESP_LOGI(TAG, "Deteniendo y limpiando animación de idle.");
    g_is_idle_running = false;
    if (g_anim_timer) {
        lv_timer_del(g_anim_timer);
        g_anim_timer = NULL;
    }
    animation_loader_free(&s_idle_animation_player);
    g_current_frame_index = -1;
}

void ui_idle_animation_pause(void) {
    if (g_anim_timer) {
        lv_timer_pause(g_anim_timer);
        g_is_idle_running = false;
        ESP_LOGI(TAG, "Animación de Idle PAUSADA.");
    }
}

void ui_idle_animation_resume(void) {
    if (g_anim_timer) {
        g_is_idle_running = true;
        lv_timer_resume(g_anim_timer);
        // Forzar una actualización inmediata para mostrar el idle en lugar del último frame de la acción
        idle_animation_timer_cb(g_anim_timer);
        ESP_LOGI(TAG, "Animación de Idle REANUDADA.");
    }
}
