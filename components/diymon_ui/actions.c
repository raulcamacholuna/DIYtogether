/*
 * =====================================================================================
 *       Filename:  actions.c
 *    Description:  Implementación del sistema de acciones dinámicas y basadas en datos.
 *        Version:  4.2 (Final y Correcto)
 * =====================================================================================
 */
#include "actions.h"
#include "screens.h"  // <<<--- ¡¡¡LA CORRECCIÓN MÁS IMPORTANTE!!!
#include "esp_log.h"

static const char *TAG = "DIYMON_ACTIONS";
static bool is_animation_in_progress = false;

/* --- El Catálogo Central de Acciones --- */
typedef struct {
    lv_obj_t **anim_obj_ptr;
    uint32_t duration_ms;
} diymon_action_def_t;

static const diymon_action_def_t ACTION_CATALOG[ACTION_ID_COUNT] = {
    [ACTION_ID_COMER]     = { .anim_obj_ptr = &objects.comiendo, .duration_ms = 2000 },
    [ACTION_ID_EJERCICIO] = { .anim_obj_ptr = &objects.ejercicio, .duration_ms = 4000 },
    [ACTION_ID_ATACAR]    = { .anim_obj_ptr = &objects.ataque,    .duration_ms = 3000 },
};

/* --- El Motor de Animación Genérico --- */
static void animation_finished_callback(lv_timer_t *timer) {
    lv_obj_t *anim_obj = (lv_obj_t *)timer->user_data;
    if (anim_obj) { lv_obj_add_flag(anim_obj, LV_OBJ_FLAG_HIDDEN); }
    lv_obj_clear_flag(objects.idle, LV_OBJ_FLAG_HIDDEN);
    is_animation_in_progress = false;
    lv_timer_del(timer);
}

static void start_diymon_animation(const diymon_action_def_t *anim_def) {
    if (is_animation_in_progress) return;
    if (!anim_def || !anim_def->anim_obj_ptr || !(*anim_def->anim_obj_ptr)) {
        ESP_LOGE(TAG, "Definición de animación o puntero a objeto inválido.");
        return;
    }
    ESP_LOGI(TAG, "Iniciando animación (duración: %dms)...", (int)anim_def->duration_ms);
    is_animation_in_progress = true;
    lv_obj_t *anim_obj = *anim_def->anim_obj_ptr;
    lv_obj_add_flag(objects.idle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(anim_obj, LV_OBJ_FLAG_HIDDEN);
    lv_animimg_start(anim_obj);
    lv_timer_create(animation_finished_callback, anim_def->duration_ms, anim_obj);
}

/* --- El Orquestador de Acciones --- */
void execute_diymon_action(lv_event_t *e) {
    diymon_action_id_t action_id = (diymon_action_id_t)lv_event_get_user_data(e);
    ESP_LOGI(TAG, "Evento dinámico recibido para la acción ID: %d", action_id);
    if (action_id < ACTION_ID_COUNT) {
        start_diymon_animation(&ACTION_CATALOG[action_id]);
    }
}

/* --- Funciones de Compatibilidad con EEZ Studio --- */
void action_comer(lv_event_t *e) { /* Lógica reemplazada */ }
void action_ejercicio(lv_event_t *e) { /* Lógica reemplazada */ }
void action_atacar(lv_event_t *e) { /* Lógica reemplazada */ }
void action_idle(lv_event_t *e) { /* Lógica reemplazada */ }