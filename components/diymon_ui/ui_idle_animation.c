/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.c
 * Fecha: 13/08/2025 - 09:34 
 * Último cambio: Refactorizado para usar el objeto de imagen y búfer compartidos.
 * Descripción: La animación de reposo ya no crea su propio objeto de imagen. En su lugar, utiliza el g_animation_img_obj global creado por el módulo de animaciones de acción. Esto soluciona conflictos de renderizado y asegura el uso de un único búfer de memoria.
 */
#include "ui_idle_animation.h"
#include "ui_action_animations.h" // Necesario para obtener el reproductor y el objeto de imagen compartidos
#include "animation_loader.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "UI_IDLE_ANIM";

#define IDLE_FRAME_INTERVAL 1500

// Variables estáticas para la gestión de la animación de reposo
static lv_timer_t *g_anim_timer;
static animation_t g_idle_animation_player; // Player local que usa el búfer compartido
static int g_current_frame_index = -1;

static void idle_animation_timer_cb(lv_timer_t *timer) {
    if (g_idle_animation_player.frame_count == 0) return;
    
    g_current_frame_index = (g_current_frame_index + 1) % g_idle_animation_player.frame_count;
    
    if (animation_loader_load_frame(&g_idle_animation_player, g_current_frame_index, "ANIM_IDLE_")) {
        lv_obj_invalidate(g_animation_img_obj); // Invalida el objeto de imagen compartido
    }
}

void ui_idle_animation_start(void) {
    animation_t* shared_player = ui_action_animations_get_player();
    if (shared_player == NULL || shared_player->img_dsc.data == NULL) {
        ESP_LOGE(TAG, "No se puede iniciar la animación idle: el búfer compartido no es válido.");
        return;
    }

    // Apuntar al búfer de datos del descriptor de imagen compartido
    g_idle_animation_player.img_dsc = shared_player->img_dsc;

    char anim_path[128];
    ui_helpers_build_asset_path(anim_path, sizeof(anim_path), "");
    size_t len = strlen(anim_path);
    if (len > 0 && anim_path[len - 1] == '/') anim_path[len - 1] = '\0';
    
    g_idle_animation_player.base_path = strdup(anim_path);
    g_idle_animation_player.frame_count = animation_loader_count_frames(anim_path, "ANIM_IDLE_");
    
    if(g_idle_animation_player.frame_count == 0) {
        ESP_LOGW(TAG, "No se encontraron fotogramas para 'ANIM_IDLE_' en %s.", anim_path);
    }
    
    // El objeto de imagen ya existe, no se crea uno nuevo.
    // Se crea el temporizador que cargará los fotogramas y los dibujará.
    g_anim_timer = lv_timer_create(idle_animation_timer_cb, IDLE_FRAME_INTERVAL, NULL);
    lv_timer_ready(g_anim_timer); // Ejecuta el primer ciclo inmediatamente para cargar el frame inicial
    
    ESP_LOGI(TAG, "Animación de Idle iniciada desde %s con %d fotogramas (usando búfer compartido).", anim_path, g_idle_animation_player.frame_count);
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
