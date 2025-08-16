/* Fecha: 16/08/2025 - 07:06  */
/* Fichero: components/diymon_ui/ui_telemetry.c */
/* Último cambio: Movida la telemetría a la esquina inferior derecha para evitar solapamiento con botones. */
/* Descripción: Implementa la lógica para mostrar la telemetría (batería, código EVO) en la pantalla principal. Crea un temporizador para actualizaciones periódicas y proporciona una función para forzar una actualización. */

#include "ui_telemetry.h"
#include "esp_log.h"
//#include "diymon_vitals.h" // Temporalmente deshabilitado para corregir la compilación
#include "diymon_evolution.h"

static const char *TAG = "UI_TELEMETRY";

/* Variables estáticas para los objetos de la UI y el temporizador */
static lv_obj_t *s_battery_label = NULL;
static lv_obj_t *s_evo_label = NULL;
static lv_timer_t *s_telemetry_timer = NULL;

/**
 * @brief Callback del temporizador para actualizar periódicamente los datos de telemetría.
 */
static void telemetry_update_timer_cb(lv_timer_t *timer) {
    if (s_battery_label) {
        //uint8_t battery_level = diymon_vitals_get_battery_percentage(); // TODO: Re-integrar diymon_vitals
        uint8_t battery_level = 100; // Valor estático para permitir la compilación
        lv_label_set_text_fmt(s_battery_label, "%s %d%%", LV_SYMBOL_BATTERY_FULL, battery_level);
    }
    if (s_evo_label) {
        const char* evo_code = diymon_get_current_code();
        lv_label_set_text_fmt(s_evo_label, "EVO: %s", evo_code ? evo_code : "N/A");
    }
}

void ui_telemetry_create(lv_obj_t *parent) {
    if (!parent) {
        ESP_LOGE(TAG, "El objeto padre es nulo. No se puede crear la telemetría.");
        return;
    }

    // Contenedor para la telemetría en la esquina inferior derecha
    lv_obj_t * telemetry_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(telemetry_cont);
    lv_obj_set_size(telemetry_cont, 120, 50);
    lv_obj_align(telemetry_cont, LV_ALIGN_BOTTOM_RIGHT, -5, -5); // Movido abajo
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

    // Crear y configurar el temporizador para la actualización periódica
    if (!s_telemetry_timer) {
        s_telemetry_timer = lv_timer_create(telemetry_update_timer_cb, 10000, NULL); // 10 segundos
        ESP_LOGI(TAG, "Temporizador de telemetría creado.");
    }
    
    // Forzar una actualización inicial
    telemetry_update_timer_cb(NULL);
}

void ui_telemetry_destroy(void) {
    if (s_telemetry_timer) {
        lv_timer_del(s_telemetry_timer);
        s_telemetry_timer = NULL;
        ESP_LOGI(TAG, "Temporizador de telemetría destruido.");
    }
    // LVGL se encarga de los hijos cuando se destruye el padre (la pantalla)
    s_battery_label = NULL;
    s_evo_label = NULL;
}

void ui_telemetry_force_update(void) {
    if (s_telemetry_timer) {
        ESP_LOGI(TAG, "Forzando actualización de telemetría.");
        // Llamamos directamente al callback para refrescar la UI al instante.
        telemetry_update_timer_cb(s_telemetry_timer);
    } else {
        ESP_LOGW(TAG, "No se puede forzar la actualización: el temporizador no existe.");
    }
}
