/*
 * Fichero: ./components/diymon_ui/actions.c
 * Fecha: 12/08/2025 - 11:15
 * Último cambio: Corregida la creación de temporizadores de un solo uso.
 * Descripción: Se reemplaza la llamada inexistente `lv_timer_create_once` por el método estándar de LVGL: `lv_timer_create` seguido de `lv_timer_set_repeat_count(timer, 1)`.
 */
#include "actions.h"
#include "ui_action_animations.h" 
#include "esp_log.h"
#include "diymon_evolution.h"
#include "bsp_api.h"
#include "ui.h"
#include "screens.h"

static const char *TAG = "DIYMON_ACTIONS";

// Declaración de la función estática que recargará la UI
static void reload_ui_task(lv_timer_t *timer);

// Implementación de la función que recargará la UI
static void reload_ui_task(lv_timer_t *timer) {
    ESP_LOGI(TAG, "Recargando UI para reflejar la nueva evolución...");
    delete_screen_main();
    ui_init();
    // El temporizador se elimina automáticamente al ser de un solo uso.
}

static void trigger_ui_reload(void) {
    lv_timer_t *timer = lv_timer_create(reload_ui_task, 10, NULL);
    lv_timer_set_repeat_count(timer, 1);
}

// La función ahora recibe los datos directamente, simplificando la lógica.
void execute_diymon_action(diymon_action_id_t action_id, lv_obj_t* idle_obj) {
    const char* current_code;
    const char* next_code;

    switch(action_id) {
        // --- Acciones de Jugador (Animaciones) ---
        case ACTION_ID_COMER:
        case ACTION_ID_EJERCICIO:
        case ACTION_ID_ATACAR:
            ESP_LOGI(TAG, "Evento de acción de jugador recibido para ID: %d. Delegando a reproductor.", action_id);
            ui_action_animations_play(action_id, idle_obj);
            break;

        // --- Acciones de Administración (Lógica + Refresco) ---
        case ACTION_ID_LVL_UP:
            current_code = diymon_get_current_code();
            next_code = diymon_get_next_evolution_in_sequence(current_code);
            if (next_code) {
                ESP_LOGI(TAG, "Acción: Subir Nivel. De '%s' a '%s'.", current_code, next_code);
                diymon_set_current_code(next_code);
                trigger_ui_reload();
            } else {
                ESP_LOGW(TAG, "Acción: Subir Nivel. Ya está en la evolución máxima.");
            }
            break;

        case ACTION_ID_LVL_DOWN:
            current_code = diymon_get_current_code();
            next_code = diymon_get_previous_evolution_in_sequence(current_code);
            if (next_code) {
                ESP_LOGI(TAG, "Acción: Bajar Nivel. De '%s' a '%s'.", current_code, next_code);
                diymon_set_current_code(next_code);
                trigger_ui_reload();
            } else {
                ESP_LOGW(TAG, "Acción: Bajar Nivel. Ya está en la evolución mínima.");
            }
            break;
            
        case ACTION_ID_SCREEN_OFF:
            ESP_LOGI(TAG, "Acción: Apagar pantalla.");
            // bsp_display_turn_off();
            break;

        default:
            ESP_LOGW(TAG, "ID de acción desconocido: %d", action_id);
            break;
    }
}