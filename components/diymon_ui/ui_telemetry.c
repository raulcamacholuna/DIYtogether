/* Fecha: 15/08/2025 - 10:07  */
/* Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_telemetry.c */
/* Último cambio: Añadida la visualización del código de evolución en la UI. */
/* Descripción: Se ha añadido un nuevo label en la esquina inferior derecha para mostrar el código de evolución actual (ej: "EVO: 1.1.2"). Este label se actualiza periódicamente junto con el indicador de batería. */
#include "ui_telemetry.h"
#include "bsp_api.h"
#include "screen_manager.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include <math.h>
#include "diymon_evolution.h" // Necesario para obtener el código de evolución

static const char *TAG = "UI_TELEMETRY";

// --- Constantes de configuración ---
#define TELEMETRY_UPDATE_INTERVAL_MS 10000
#define MIN_BAT_V                    3.0f
#define MAX_BAT_V                    4.2f

// --- Variables estáticas del módulo ---
static lv_obj_t *s_battery_label;
static lv_obj_t *s_evo_label; // Nuevo label para la evolución
static lv_timer_t *s_telemetry_timer;

/**
 * @brief Callback del temporizador que actualiza los datos de telemetría en la pantalla.
 */
static void telemetry_update_timer_cb(lv_timer_t *timer) {
    float voltage;

    // Proteger el acceso I2C con el mutex de LVGL
    if (lvgl_port_lock(0)) {
        bsp_battery_get_voltage(&voltage, NULL);
        lvgl_port_unlock();
    } else {
        return; // No se pudo obtener el mutex, reintentar en el próximo ciclo
    }

    // Procesar y mostrar datos de batería
    float percentage = 100.0f * (voltage - MIN_BAT_V) / (MAX_BAT_V - MIN_BAT_V);
    if (percentage > 100.0f) percentage = 100.0f;
    if (percentage < 0.0f) percentage = 0.0f;
    lv_label_set_text_fmt(s_battery_label, LV_SYMBOL_BATTERY_FULL " %d%%", (int)percentage);

    // [NUEVO] Procesar y mostrar datos de evolución
    const char* evo_code = diymon_get_current_code();
    lv_label_set_text_fmt(s_evo_label, "EVO: %s", evo_code);
}

void ui_telemetry_create(lv_obj_t *parent) {
    // Estilo para el label de telemetría
    static lv_style_t style_telemetry;
    lv_style_init(&style_telemetry);
    lv_style_set_text_color(&style_telemetry, lv_color_black());
    lv_style_set_text_font(&style_telemetry, &lv_font_montserrat_14);
    lv_style_set_bg_color(&style_telemetry, lv_color_white());
    lv_style_set_bg_opa(&style_telemetry, LV_OPA_70);
    lv_style_set_radius(&style_telemetry, 5);
    lv_style_set_pad_all(&style_telemetry, 5);

    // Crear el label de la batería
    s_battery_label = lv_label_create(parent);
    lv_obj_add_style(s_battery_label, &style_telemetry, 0);
    lv_obj_align(s_battery_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    lv_label_set_text(s_battery_label, LV_SYMBOL_BATTERY_FULL " --%");

    // [NUEVO] Crear el label de Evolución
    s_evo_label = lv_label_create(parent);
    lv_obj_add_style(s_evo_label, &style_telemetry, 0);
    lv_obj_align(s_evo_label, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
    lv_label_set_text(s_evo_label, "EVO: ---");
    
    // Crear y lanzar el temporizador
    s_telemetry_timer = lv_timer_create(telemetry_update_timer_cb, TELEMETRY_UPDATE_INTERVAL_MS, NULL);
    telemetry_update_timer_cb(s_telemetry_timer); // Llamada inicial para poblar los campos
    ESP_LOGI(TAG, "Módulo de telemetría de UI (batería y evolución) creado.");
}

void ui_telemetry_destroy(void) {
    if (s_telemetry_timer) {
        lv_timer_del(s_telemetry_timer);
        s_telemetry_timer = NULL;
        ESP_LOGI(TAG, "Temporizador de telemetría destruido.");
    }
}
