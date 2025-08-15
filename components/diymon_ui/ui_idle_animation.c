/* Fecha: 15/08/2025 - 09:49  */
/* Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_idle_animation.c */
/* Último cambio: Implementada la detección dinámica del número de fotogramas de la animación de reposo. */
/* Descripción: Se ha modificado la función `ui_idle_animation_start` para que cuente cuántos ficheros `ANIM_IDLE_*.bin` existen en el directorio de assets actual. Este recuento se utiliza como el número de fotogramas, eliminando la constante `IDLE_ANIM_FRAME_COUNT` y permitiendo que las animaciones de reposo tengan una duración variable. */

#include "ui_idle_animation.h"
#include "ui_action_animations.h" 
#include "animation_loader.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "UI_IDLE_ANIM";

#define IDLE_FRAME_INTERVAL 1500

static lv_timer_t *g_anim_timer;
static animation_t s_idle_animation_player; // Player local, pero usará un búfer compartido
static int g_current_frame_index = -1;
static bool g_is_idle_running = false;

static void idle_animation_timer_cb(lv_timer_t *timer) {
    if (!g_is_idle_running || s_idle_animation_player.frame_count == 0) return;
    
    g_current_frame_index = (g_current_frame_index + 1) % s_idle_animation_player.frame_count;
    
    if (animation_loader_load_frame(&s_idle_animation_player, g_current_frame_index, "ANIM_IDLE_")) {
        if (g_animation_img_obj) {
            lv_image_set_src(g_animation_img_obj, &s_idle_animation_player.img_dsc);
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

    // Copiar el descriptor, que incluye el puntero al búfer compartido.
    s_idle_animation_player.img_dsc = shared_player->img_dsc;

    char anim_path[128];
    ui_helpers_build_asset_path(anim_path, sizeof(anim_path), "");
    size_t len = strlen(anim_path);
    if (len > 0 && anim_path[len - 1] == '/') anim_path[len - 1] = '\0';
    
    s_idle_animation_player.base_path = strdup(anim_path);

    // [CAMBIO CLAVE] Contar dinámicamente los fotogramas de la animación de reposo.
    uint16_t frame_count = animation_loader_count_frames(anim_path, "ANIM_IDLE_");
    if (frame_count == 0) {
        ESP_LOGE(TAG, "No se encontraron fotogramas para la animación de reposo en '%s'. La animación no se iniciará.", anim_path);
        free(s_idle_animation_player.base_path);
        s_idle_animation_player.base_path = NULL;
        return g_animation_img_obj; // Devolver el objeto de imagen aunque esté vacío para no romper la UI.
    }
    ESP_LOGI(TAG, "Detectados %d fotogramas para la animación de reposo.", frame_count);
    s_idle_animation_player.frame_count = frame_count;

    // El objeto de imagen ya existe, solo nos aseguramos de que apunte a nuestros datos.
    if(g_animation_img_obj) {
        lv_image_set_src(g_animation_img_obj, &s_idle_animation_player.img_dsc);
    }
    
    g_is_idle_running = true;
    g_anim_timer = lv_timer_create(idle_animation_timer_cb, IDLE_FRAME_INTERVAL, NULL);
    lv_timer_ready(g_anim_timer);
    
    ESP_LOGI(TAG, "Animación de Idle iniciada desde %s (usando búfer compartido).", anim_path);
    return g_animation_img_obj;
}

void ui_idle_animation_stop(void) {
    ESP_LOGI(TAG, "Deteniendo y limpiando animación de idle.");
    g_is_idle_running = false;
    if (g_anim_timer) {
        lv_timer_del(g_anim_timer);
        g_anim_timer = NULL;
    }
    // Solo liberar la ruta, no el búfer de datos que es compartido.
    if (s_idle_animation_player.base_path) {
        free(s_idle_animation_player.base_path);
        s_idle_animation_player.base_path = NULL;
    }
    g_current_frame_index = -1;
}

void ui_idle_animation_pause(void) {
    if (g_anim_timer && g_is_idle_running) {
        lv_timer_pause(g_anim_timer);
        g_is_idle_running = false;
        ESP_LOGI(TAG, "Animación de Idle PAUSADA.");
    }
}

void ui_idle_animation_resume(void) {
    if (g_anim_timer && !g_is_idle_running) {
        // Asegurarse de que el objeto de imagen apunta al descriptor de la animación de reposo
        if (g_animation_img_obj) {
             lv_image_set_src(g_animation_img_obj, &s_idle_animation_player.img_dsc);
        }
        
        g_is_idle_running = true;
        lv_timer_resume(g_anim_timer);
        // Forzar una actualización inmediata para mostrar el idle en lugar del último frame de la acción
        idle_animation_timer_cb(g_anim_timer);
        ESP_LOGI(TAG, "Animación de Idle REANUDADA.");
    }
}
