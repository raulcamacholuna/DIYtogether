/* Fecha: 17/08/2025 - 02:34  */
/* Fichero: components/ui/actions/action_evolution.c */
/* Último cambio: Actualizadas las llamadas al gestor de telemetría para reflejar la refactorización a ui/core/. */
/* Descripción: Módulo que maneja los cambios de estado del Diymon. Se ha actualizado para llamar a telemetry_manager_update_values en lugar de la antigua función, adaptándose a la nueva estructura del núcleo de la UI. */

#include "actions/action_evolution.h"
#include "diymon_evolution.h"
#include "ui_idle_animation.h"
#include "telemetry_manager.h" // Actualizado desde ui_telemetry.h
#include "ui_action_animations.h" // Para g_animation_img_obj
#include "screens.h"              // Para g_main_screen_obj
#include "esp_log.h"

static const char *TAG = "ACTION_EVOLUTION";

/**
 * @brief Función interna para actualizar la UI después de un cambio de evolución.
 */
static void update_ui_after_evolution_change(void) {
    // Detiene la animación de reposo actual para poder recargarla con el nuevo personaje.
    ui_idle_animation_stop();
    // Oculta temporalmente el objeto de la animación para evitar que se vea el último fotograma del personaje anterior.
    if (g_animation_img_obj) {
        lv_obj_add_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
    }

    // Fuerza la actualización inmediata de la telemetría para que muestre el nuevo código EVO.
    telemetry_manager_update_values(100); // Se asume 100% de batería, ya que no se recalcula aquí. La tarea principal lo corregirá.
    
    // Inicia la nueva animación de reposo, que cargará los assets del nuevo código de evolución.
    ui_idle_animation_start(g_main_screen_obj);
    
    // Vuelve a hacer visible el objeto de la animación.
    if (g_animation_img_obj) {
        lv_obj_clear_flag(g_animation_img_obj, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
 * @brief Implementa la acción de evolucionar a una rama.
 */
void action_evolution_branch(int branch_id) {
    const char* current_code = diymon_get_current_code();
    const char* next_code = diymon_get_branched_evolution(current_code, branch_id);

    if (next_code) {
        ESP_LOGI(TAG, "Evolucionando de '%s' a '%s' (rama %d)", current_code, next_code, branch_id);
        diymon_set_current_code(next_code);
        update_ui_after_evolution_change();
    } else {
        ESP_LOGW(TAG, "Evolución inválida desde '%s' por la rama %d", current_code, branch_id);
    }
}

/**
 * @brief Implementa la acción de involucionar.
 */
void action_evolution_devolve(void) {
    const char* current_code = diymon_get_current_code();
    const char* previous_code = diymon_get_previous_evolution_in_sequence(current_code);

    if (previous_code) {
        ESP_LOGI(TAG, "Involucionando de '%s' a '%s'", current_code, previous_code);
        diymon_set_current_code(previous_code);
        update_ui_after_evolution_change();
    } else {
        ESP_LOGW(TAG, "Involución inválida desde '%s' (ya es la forma base)", current_code);
    }
}
