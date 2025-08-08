#include "actions.h"
#include "ui_priv.h"
#include "esp_log.h"

static const char *TAG = "DIYMON_ACTIONS";

// Semaforo para asegurar que solo una acción se ejecuta a la vez.
static bool is_action_in_progress = false;

/**
 * @brief Estructura de definición para una acción de DIYMON.
 */
typedef struct {
    lv_obj_t **action_obj_ptr; // Puntero al objeto de imagen de la acción.
    uint32_t duration_ms;      // Duración que la imagen permanecerá visible.
} diymon_action_def_t;

// Catálogo de acciones disponibles, asociando cada ID con su objeto y duración.
static const diymon_action_def_t ACTION_CATALOG[ACTION_ID_COUNT] = {
    [ACTION_ID_COMER]     = { .action_obj_ptr = &objects.comiendo, .duration_ms = 2500 },
    [ACTION_ID_EJERCICIO] = { .action_obj_ptr = &objects.ejercicio, .duration_ms = 4000 },
    [ACTION_ID_ATACAR]    = { .action_obj_ptr = &objects.ataque,    .duration_ms = 3000 },
};

/**
 * @brief Callback ejecutado por el temporizador al finalizar una acción.
 * 
 * Oculta la imagen de la acción, reanuda la animación de reposo y libera el
 * semáforo para permitir nuevas acciones.
 * @param timer Puntero al temporizador que invocó el callback.
 */
static void action_finished_callback(lv_timer_t *timer) {
    // Recupera el objeto de imagen de la acción desde los datos de usuario del timer.
    lv_obj_t *action_obj = (lv_obj_t *)timer->user_data;
    
    // Oculta la imagen de la acción.
    lv_obj_add_flag(action_obj, LV_OBJ_FLAG_HIDDEN);
    
    // Muestra y reanuda la animación de reposo (idle).
    if (objects.idle) {
        lv_obj_clear_flag(objects.idle, LV_OBJ_FLAG_HIDDEN);
        lv_gif_resume(objects.idle);
    }
    
    // Libera el semáforo para permitir la ejecución de la siguiente acción.
    is_action_in_progress = false;
    
    // Elimina el temporizador, ya que es de un solo uso.
    lv_timer_del(timer);
}

/**
 * @brief Inicia la visualización de una imagen de acción.
 * 
 * Pausa la animación de reposo, muestra la imagen de la acción especificada
 * y crea un temporizador para ocultarla después de la duración definida.
 * @param action_def Puntero a la definición de la acción a ejecutar.
 */
static void start_diymon_action_display(const diymon_action_def_t *action_def) {
    if (is_action_in_progress) {
        ESP_LOGW(TAG, "Se ha intentado iniciar una acción mientras otra estaba en progreso.");
        return;
    }
    if (!action_def || !action_def->action_obj_ptr || !(*action_def->action_obj_ptr)) {
        ESP_LOGE(TAG, "Se ha intentado iniciar una acción con una definición o un objeto inválido.");
        return;
    }

    ESP_LOGI(TAG, "Iniciando acción (duración: %dms)...", (int)action_def->duration_ms);
    is_action_in_progress = true;
    
    lv_obj_t *action_img_obj = *action_def->action_obj_ptr;
    
    // Pausa y oculta la animación de reposo.
    if (objects.idle) {
        lv_gif_pause(objects.idle);
        lv_obj_add_flag(objects.idle, LV_OBJ_FLAG_HIDDEN);
    }
    
    // Muestra la imagen de la acción.
    lv_obj_clear_flag(action_img_obj, LV_OBJ_FLAG_HIDDEN);
    
    // Crea un temporizador de un solo uso para revertir los cambios.
    lv_timer_create(action_finished_callback, action_def->duration_ms, action_img_obj);
}

/**
 * @brief Orquestador de acciones, llamado por los eventos de los botones.
 * 
 * Recibe un evento, extrae el ID de la acción y llama a la función que
 * gestiona la visualización de la imagen correspondiente.
 * @param e Puntero al evento de LVGL que contiene el ID de la acción.
 */
void execute_diymon_action(lv_event_t *e) {
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    ESP_LOGI(TAG, "Evento de acción recibido para el ID: %d", action_id);
    
    if (action_id < ACTION_ID_COUNT) {
        start_diymon_action_display(&ACTION_CATALOG[action_id]);
    } else {
        ESP_LOGE(TAG, "ID de acción (%d) fuera de rango.", action_id);
    }
}