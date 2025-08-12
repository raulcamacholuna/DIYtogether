/*
 * Fichero: ./components/diymon_ui/ui_action_animations.c
 * Fecha: 14/08/2025 - 10:00 am
 * Último cambio: Implementada la lógica de búfer de animación compartido.
 * Descripción: Gestión de las animaciones de acción (comer, etc.). Ahora asigna un único búfer de memoria que será compartido con la animación de reposo. Controla la pausa y reanudación de la animación de reposo para evitar conflictos de dibujado.
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
animation_t g_animation_player;

static lv_timer_t *s_anim_timer;
static bool s_is_action_in_progress = false;
static int s_current_frame_index;

// Velocidad de la animación en milisegundos por fotograma.
#define FRAME_INTERVAL_MS 500

// --- Declaraciones de Funciones Internas ---
static void animation_timer_cb(lv_timer_t *timer);
static void animation_finished(void);
static const char* get_anim_prefix(diymon_action_id_t action_id);

// --- Implementación de Funciones Públicas ---

void ui_action_animations_create(lv_obj_t *parent) {
    // 1. Inicializar el reproductor compartido. Esto reserva el búfer de memoria una sola vez.
    g_animation_player = animation_loader_init(NULL, 170, 320, 0);
    if (g_animation_player.img_dsc.data == NULL) {
        ESP_LOGE(TAG, "FALLO CRÍTICO: No se pudo reservar memoria para el búfer de animación compartido.");
        return;
    }
    ESP_LOGI(TAG, "Búfer de animación compartido pre-reservado correctamente.");

    // 2. Crear el objeto de imagen que actuará como lienzo para todas las animaciones.
    g_animation_img_obj = lv_img_create(parent);
    lv_img_set_src(g_animation_img_obj, &g_animation_player.img_dsc);
    lv_obj_align(g_animation_img_obj, LV_ALIGN_CENTER, 0, 0);
    
    // Se mueve al fondo para que los botones y otros elementos de la UI aparezcan por encima.
    lv_obj_move_background(g_animation_img_obj);
}

void ui_action_animations_play(diymon_action_id_t action_id) {
    if (s_is_action_in_progress || action_id >= ACTION_ID_COUNT) return;
    if (g_animation_player.img_dsc.data == NULL) {
        ESP_LOGE(TAG, "No se puede iniciar la animación: el búfer compartido no está disponible.");
        return;
    }

    s_is_action_in_progress = true;
    
    // Pausar la animación de reposo para tomar control del búfer
    ui_idle_animation_pause();

    const char *prefix = get_anim_prefix(action_id);
    char path_buffer[128];
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "");
    size_t len = strlen(path_buffer);
    if (len > 0 && path_buffer[len - 1] == '/') path_buffer[len - 1] = '\0';
    
    uint16_t frame_count = animation_loader_count_frames(path_buffer, prefix);
    if (frame_count == 0) {
        ESP_LOGE(TAG, "No se encontraron fotogramas para la animación '%s' en '%s'.", prefix, path_buffer);
        animation_finished(); // Llama a finished para reanudar idle y limpiar estado.
        return;
    }
    
    ESP_LOGI(TAG, "Reproduciendo animación '%s' (%d fotogramas) a %dms/frame.", prefix, frame_count, FRAME_INTERVAL_MS);

    if (g_animation_player.base_path) free(g_animation_player.base_path);
    g_animation_player.base_path = strdup(path_buffer);
    g_animation_player.frame_count = frame_count;

    s_current_frame_index = 0;
    if (animation_loader_load_frame(&g_animation_player, s_current_frame_index, prefix)) {
        lv_obj_invalidate(g_animation_img_obj); // Redibuja el objeto de imagen con el nuevo frame.
        s_anim_timer = lv_timer_create(animation_timer_cb, FRAME_INTERVAL_MS, (void*)(intptr_t)action_id);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el primer fotograma (%s).", prefix);
        animation_finished();
    }
}

void ui_action_animations_destroy(void) {
    ESP_LOGI(TAG, "Liberando búfer de animación compartido.");
    animation_loader_free(&g_animation_player);
    // El objeto g_animation_img_obj es hijo del screen y LVGL lo destruirá automáticamente.
}

animation_t* ui_action_animations_get_player(void) {
    return &g_animation_player;
}


// --- Implementación de Funciones Internas ---

static void animation_timer_cb(lv_timer_t *timer) {
    s_current_frame_index++;
    if (s_current_frame_index >= g_animation_player.frame_count) {
        animation_finished();
        return;
    }

    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)timer->user_data;
    const char *prefix = get_anim_prefix(action_id);

    if (animation_loader_load_frame(&g_animation_player, s_current_frame_index, prefix)) {
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
    
    if (g_animation_player.base_path) {
        free(g_animation_player.base_path);
        g_animation_player.base_path = NULL;
    }
    g_animation_player.frame_count = 0;
    
    // Devuelve el control del búfer a la animación de reposo.
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