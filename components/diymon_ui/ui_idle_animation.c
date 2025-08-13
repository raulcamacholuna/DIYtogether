/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.c
 * Fecha: 13/08/2025 - 11:45 
 * Último cambio: Corregida la firma de la función de inicio.
 * Descripción: Implementación de la animación de reposo. Se corrige la firma de
 *              la función 'ui_idle_animation_start' para que coincida con su
 *              declaración, aceptando un objeto padre y devolviendo el objeto
 *              de animación creado, resolviendo así el error de compilación.
 */
#include "ui_idle_animation.h"
#include "ui_action_animations.h" // Necesario para obtener el reproductor compartido
#include "animation_loader.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "UI_IDLE_ANIM";

#define IDLE_ANIM_FRAME_COUNT 3
#define IDLE_FRAME_INTERVAL 1500

// Variables estáticas para la gestión de la animación de reposo
static lv_timer_t *g_anim_timer;
static animation_t g_idle_animation_player; // Player local que usa el búfer compartido
static int g_current_frame_index = -1;

static void idle_animation_timer_cb(lv_timer_t *timer) {
    if (g_idle_animation_player.frame_count == 0) return;
    
    g_current_frame_index = (g_current_frame_index + 1) % g_idle_animation_player.frame_count;
    
    if (animation_loader_load_frame(&g_idle_animation_player, g_current_frame_index, "IDLE_")) {
        // g_animation_img_obj es el objeto global declarado en ui_action_animations.h
        if (g_animation_img_obj) {
            lv_obj_invalidate(g_animation_img_obj);
        }
    }
}

lv_obj_t* ui_idle_animation_start(lv_obj_t *parent) {
    animation_t* shared_player = ui_action_animations_get_player();
    if (shared_player == NULL || shared_player->img_dsc.data == NULL) {
        ESP_LOGE(TAG, "No se puede iniciar la animación idle: el búfer compartido no es válido.");
        return NULL;
    }

    g_idle_animation_player.img_dsc = shared_player->img_dsc;

    char anim_path[128];
    ui_helpers_build_asset_path(anim_path, sizeof(anim_path), "");
    size_t len = strlen(anim_path);
    if (len > 0 && anim_path[len - 1] == '/') anim_path[len - 1] = '\0';
    
    g_idle_animation_player.base_path = strdup(anim_path);
    g_idle_animation_player.frame_count = IDLE_ANIM_FRAME_COUNT;

    // g_animation_img_obj es el objeto de imagen compartido. Lo usamos para mostrar la animación.
    // Ya fue creado en ui_action_animations_create, así que no lo creamos de nuevo.
    // Solo nos aseguramos de que sea visible y esté en el fondo.
    if(g_animation_img_obj) {
        lv_obj_move_background(g_animation_img_obj);
    }
    
    g_anim_timer = lv_timer_create(idle_animation_timer_cb, IDLE_FRAME_INTERVAL, NULL);
    lv_timer_ready(g_anim_timer);
    
    ESP_LOGI(TAG, "Animación de Idle iniciada desde %s (usando búfer compartido).", anim_path);
    return g_animation_img_obj; // Devolvemos el puntero al objeto de imagen compartido
}

void ui_idle_animation_stop(void) {
    ESP_LOGI(TAG, "Deteniendo y limpiando animación de idle.");
    if (g_anim_timer) {
        lv_timer_del(g_anim_timer);
        g_anim_timer = NULL;
    }
    if (g_idle_animation_player.base_path) {
        free(g_idle_animation_player.base_path);
        g_idle_animation_player.base_path = NULL;
    }
    g_current_frame_index = -1;
}

void ui_idle_animation_pause(void) {
    if (g_anim_timer) {
        lv_timer_pause(g_anim_timer);
        ESP_LOGI(TAG, "Animación de Idle PAUSADA.");
    }
}

void ui_idle_animation_resume(void) {
    if (g_anim_timer) {
        lv_timer_resume(g_anim_timer);
        ESP_LOGI(TAG, "Animación de Idle REANUDADA.");
    }
}
