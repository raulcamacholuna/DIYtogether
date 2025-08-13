/*
  Fichero: ./components/diymon_ui/ui_telemetry.c
  Fecha: 13/08/2025 - 12:19 
  Último cambio: Invertidos los colores y movidos los labels a la esquina inferior.
  Descripción: Se ha modificado el estilo de los labels de telemetría para que tengan
               texto negro sobre fondo blanco semitransparente. Además, se han realineado
               todos los elementos en la esquina inferior izquierda de la pantalla para
               mejorar la visibilidad de la animación principal.
*/
#include "ui_telemetry.h"
#include "bsp_api.h"
#include "screen_manager.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include <math.h>

static const char *TAG = "UI_TELEMETRY";

// --- Constantes de configuración ---
#define TELEMETRY_UPDATE_INTERVAL_MS 10000
#define SHAKE_THRESHOLD_G            2.5f
#define MIN_BAT_V                    3.0f
#define MAX_BAT_V                    4.2f

// --- Variables estáticas del módulo ---
static lv_obj_t *s_battery_label;
static lv_obj_t *s_orientation_label;
static lv_obj_t *s_accel_label;
static lv_timer_t *s_telemetry_timer;

/**
 * @brief Callback del temporizador que actualiza los datos de telemetría en la pantalla.
 */
static void telemetry_update_timer_cb(lv_timer_t *timer) {
    float acc[3], gyro[3];
    float voltage;

    // Proteger el acceso I2C con el mutex de LVGL
    if (lvgl_port_lock(0)) {
        bsp_imu_read(acc, gyro);
        bsp_battery_get_voltage(&voltage, NULL);
        lvgl_port_unlock();
    } else {
        return; // No se pudo obtener el mutex, reintentar en el próximo ciclo
    }

    // 1. Procesar y mostrar datos de batería
    float percentage = 100.0f * (voltage - MIN_BAT_V) / (MAX_BAT_V - MIN_BAT_V);
    if (percentage > 100.0f) percentage = 100.0f;
    if (percentage < 0.0f) percentage = 0.0f;
    lv_label_set_text_fmt(s_battery_label, LV_SYMBOL_BATTERY_FULL " %d%%", (int)percentage);

    // 2. Procesar y mostrar datos de aceleración
    float magnitude = sqrt(acc[0] * acc[0] + acc[1] * acc[1] + acc[2] * acc[2]);
    lv_label_set_text_fmt(s_accel_label, LV_SYMBOL_CHARGE " %.2f m/s2", magnitude);

    // 3. Procesar y mostrar datos de orientación
    float roll = atan2(acc[1], acc[2]) * 180.0 / M_PI;
    float pitch = atan2(-acc[0], sqrt(acc[1] * acc[1] + acc[2] * acc[2])) * 180.0 / M_PI;
    lv_label_set_text_fmt(s_orientation_label, LV_SYMBOL_SETTINGS " R:%.0f P:%.0f", roll, pitch);

    // 4. Lógica de 'shake-to-wake'
    if (screen_manager_is_off() && (magnitude > (SHAKE_THRESHOLD_G * 9.81f))) {
        ESP_LOGI(TAG, "Shake detectado por sondeo (magnitud: %.2f > %.2f). Encendiendo pantalla.", magnitude, SHAKE_THRESHOLD_G * 9.81f);
        if (lvgl_port_lock(0)) {
            screen_manager_turn_on();
            lvgl_port_unlock();
        }
    }
}

void ui_telemetry_create(lv_obj_t *parent) {
    // Estilo para los labels de telemetría (colores invertidos)
    static lv_style_t style_telemetry;
    lv_style_init(&style_telemetry);
    lv_style_set_text_color(&style_telemetry, lv_color_black());
    lv_style_set_text_font(&style_telemetry, &lv_font_montserrat_14);
    lv_style_set_bg_color(&style_telemetry, lv_color_white());
    lv_style_set_bg_opa(&style_telemetry, LV_OPA_70); // Un poco más opaco para legibilidad
    lv_style_set_radius(&style_telemetry, 5);
    lv_style_set_pad_all(&style_telemetry, 5);

    // Crear los tres labels en la esquina inferior
    s_orientation_label = lv_label_create(parent);
    lv_obj_add_style(s_orientation_label, &style_telemetry, 0);
    lv_obj_align(s_orientation_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    lv_label_set_text(s_orientation_label, LV_SYMBOL_SETTINGS " R:-- P:--");

    s_battery_label = lv_label_create(parent);
    lv_obj_add_style(s_battery_label, &style_telemetry, 0);
    lv_obj_align_to(s_battery_label, s_orientation_label, LV_ALIGN_OUT_TOP_LEFT, 0, -5);
    lv_label_set_text(s_battery_label, LV_SYMBOL_BATTERY_FULL " --%");

    s_accel_label = lv_label_create(parent);
    lv_obj_add_style(s_accel_label, &style_telemetry, 0);
    lv_obj_align_to(s_accel_label, s_battery_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_label_set_text(s_accel_label, LV_SYMBOL_CHARGE " -- m/s2");
    
    // Crear y lanzar el temporizador
    s_telemetry_timer = lv_timer_create(telemetry_update_timer_cb, TELEMETRY_UPDATE_INTERVAL_MS, NULL);
    ESP_LOGI(TAG, "Módulo de telemetría de UI creado.");
}

void ui_telemetry_destroy(void) {
    if (s_telemetry_timer) {
        lv_timer_del(s_telemetry_timer);
        s_telemetry_timer = NULL;
        ESP_LOGI(TAG, "Temporizador de telemetría destruido.");
    }
}
