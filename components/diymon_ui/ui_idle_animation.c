/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.c
 * Fecha: 10/08/2025 - 21:00
 * Último cambio: Adaptado para usar el búfer de animación compartido.
 * Descripción: Este módulo ya no reserva su propio búfer de memoria. En su lugar,
 *              obtiene una referencia al búfer compartido del módulo
 *              `ui_action_animations`, resolviendo así el conflicto de memoria
 *              y el fallo de sistema `Load access fault`.
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
#define IDLE_FRAME_INTERVAL 750

// Variables estáticas para la gestión de la animación de reposo
static lv_obj_t *g_anim_img_obj;
static lv_timer_t *g_anim_timer;
static animation_t g_idle_animation_player; // Player local que usa el búfer compartido
static int g_current_frame_index = -1;

static void idle_animation_timer_cb(lv_timer_t *timer) {
    if (g_idle_animation_player.frame_count == 0) return;
    
    g_current_frame_index = (g_current_frame_index + 1) % g_idle_animation_player.frame_count;
    
    if (animation_loader_load_frame(&g_idle_animation_player, g_current_frame_index, "IDLE_")) {
        lv_obj_invalidate(g_anim_img_obj);
    }
}

lv_obj_t* ui_idle_animation_start(lv_obj_t *parent) {
    // --- SOLUCIÓN: Obtener el reproductor de animación compartido ---
    animation_t* shared_player = ui_action_animations_get_player();
    if (shared_player == NULL || shared_player->img_dsc.data == NULL) {
        ESP_LOGE(TAG, "No se puede iniciar la animación idle: el búfer compartido no es válido.");
        return NULL;
    }

    // Copiar la configuración del descriptor de imagen (que contiene el puntero al búfer)
    g_idle_animation_player.img_dsc = shared_player->img_dsc;

    // Configurar los parámetros específicos de la animación de reposo
    char anim_path[128];
    ui_helpers_build_asset_path(anim_path, sizeof(anim_path), "");
    size_t len = strlen(anim_path);
    if (len > 0 && anim_path[len - 1] == '/') anim_path[len - 1] = '\0';
    
    g_idle_animation_player.base_path = strdup(anim_path);
    g_idle_animation_player.frame_count = IDLE_ANIM_FRAME_COUNT;

    // Crear el objeto de imagen y asignarle el descriptor con el búfer compartido
    g_anim_img_obj = lv_img_create(parent);
    lv_img_set_src(g_anim_img_obj, &g_idle_animation_player.img_dsc);
    lv_obj_align(g_anim_img_obj, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_move_background(g_anim_img_obj);

    // Iniciar el temporizador para ciclar los fotogramas
    g_anim_timer = lv_timer_create(idle_animation_timer_cb, IDLE_FRAME_INTERVAL, NULL);
    lv_timer_ready(g_anim_timer);
    
    ESP_LOGI(TAG, "Animación de Idle iniciada desde %s (usando búfer compartido).", anim_path);
    return g_anim_img_obj;
}

void ui_idle_animation_stop(void) {
    ESP_LOGI(TAG, "Deteniendo y limpiando animación de idle.");
    if (g_anim_timer) {
        lv_timer_del(g_anim_timer);
        g_anim_timer = NULL;
    }
    // Liberar solo la ruta de la animación de idle. El búfer principal
    // será liberado por el módulo de animaciones de acción.
    if (g_idle_animation_player.base_path) {
        free(g_idle_animation_player.base_path);
        g_idle_animation_player.base_path = NULL;
    }
    g_current_frame_index = -1;
}