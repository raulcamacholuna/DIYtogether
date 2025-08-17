/* Fecha: 17/08/2025 - 02:31  */
/* Fichero: components/ui/core/telemetry_manager.c */
/* Último cambio: Movido a ui/core/ y renombrado de ui_telemetry.c, actualizando nombres de funciones para consistencia. */
/* Descripción: Implementación del módulo de telemetría de la UI. Ahora es un receptor pasivo de datos, actualizando los labels cuando la función 'telemetry_manager_update_values' es llamada por una tarea externa. */

#include "telemetry_manager.h"
#include "esp_log.h"
#include "diymon_evolution.h"
#include "esp_lvgl_port.h" // Necesario para el bloqueo thread-safe

static const char *TAG = "TELEMETRY_MANAGER";

/* Variables estáticas para los objetos de la UI */
static lv_obj_t *s_battery_label = NULL;
static lv_obj_t *s_evo_label = NULL;

/**
 * @brief Crea y configura los elementos de la UI para la telemetría.
 */
void telemetry_manager_create(lv_obj_t *parent) {
    if (!parent) {
        ESP_LOGE(TAG, "El objeto padre es nulo. No se puede crear la telemetría.");
        return;
    }

    // Contenedor para la telemetría en la esquina inferior derecha
    lv_obj_t * telemetry_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(telemetry_cont);
    lv_obj_set_size(telemetry_cont, 120, 50);
    lv_obj_align(telemetry_cont, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
    lv_obj_set_flex_flow(telemetry_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(telemetry_cont, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Etiqueta para el nivel de batería
    s_battery_label = lv_label_create(telemetry_cont);
    lv_obj_set_style_text_color(s_battery_label, lv_color_white(), 0);
    lv_label_set_text(s_battery_label, "Bateria...");

    // Etiqueta para el código de evolución
    s_evo_label = lv_label_create(telemetry_cont);
    lv_obj_set_style_text_color(s_evo_label, lv_color_white(), 0);
    lv_label_set_text(s_evo_label, "EVO: ...");
    
    // Forzar una actualización inicial con valores por defecto
    telemetry_manager_update_values(100);
    ESP_LOGI(TAG, "Módulo de telemetría creado.");
}

/**
 * @brief Destruye los recursos asociados al módulo de telemetría.
 */
void telemetry_manager_destroy(void) {
    // Ya no hay temporizador que borrar. LVGL se encarga de los objetos hijos.
    s_battery_label = NULL;
    s_evo_label = NULL;
    ESP_LOGI(TAG, "Módulo de telemetría destruido.");
}

/**
 * @brief Actualiza los valores mostrados en los labels de telemetría.
 */
void telemetry_manager_update_values(uint8_t battery_percentage) {
    // Bloquear el mutex de LVGL ya que esta función será llamada desde otra tarea
    if (lvgl_port_lock(0)) {
        if (s_battery_label) {
            lv_label_set_text_fmt(s_battery_label, "%s %d%%", LV_SYMBOL_BATTERY_FULL, battery_percentage);
        }
        if (s_evo_label) {
            const char* evo_code = diymon_get_current_code();
            lv_label_set_text_fmt(s_evo_label, "EVO: %s", evo_code ? evo_code : "N/A");
        }
        lvgl_port_unlock();
    }
}
