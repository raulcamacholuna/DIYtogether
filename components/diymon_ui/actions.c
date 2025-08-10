/*
 * Fichero: ./components/diymon_ui/actions.c
 * Fecha: 13/08/2025 - 09:10
 * Último cambio: Implementada la lógica (placeholder) para las nuevas acciones de evolución.
 * Descripción: Se ha modificado la lógica de los botones de subir/bajar nivel para que, además de cambiar la evolución, ajusten el brillo de la pantalla en pasos del 10%.
 */
#include "actions.h"
#include "ui_action_animations.h" 
#include "esp_log.h"
#include "diymon_evolution.h"
#include "bsp_api.h"
#include "ui.h"
#include "screens.h"
#include "screen_state.h"

static const char *TAG = "DIYMON_ACTIONS";

// Variable global de estado de la pantalla
bool g_screen_is_off = false;
// Variable estática para llevar la cuenta del brillo actual
static int s_current_brightness = 100;

static void reload_ui_task(lv_timer_t *timer) {
    ESP_LOGI(TAG, "Recargando UI para reflejar la nueva evolución...");
    delete_screen_main();
    ui_init();
}

static void trigger_ui_reload(void) {
    lv_timer_t *timer = lv_timer_create(reload_ui_task, 10, NULL);
    lv_timer_set_repeat_count(timer, 1);
}

void execute_diymon_action(diymon_action_id_t action_id, lv_obj_t* idle_obj) {
    const char* current_code;
    const char* next_code;

    switch(action_id) {
        case ACTION_ID_COMER:
        case ACTION_ID_EJERCICIO:
        case ACTION_ID_ATACAR:
            ESP_LOGI(TAG, "Evento de acción de jugador recibido para ID: %d. Delegando a reproductor.", action_id);
            ui_action_animations_play(action_id, idle_obj);
            break;

        case ACTION_ID_LVL_UP:
            // Ajustar brillo
            s_current_brightness += 10;
            if (s_current_brightness > 100) s_current_brightness = 100;
            bsp_display_set_brightness(s_current_brightness);
            ESP_LOGI(TAG, "Acción: Subir Brillo a %d%%", s_current_brightness);

            // Cambiar nivel
            current_code = diymon_get_current_code();
            next_code = diymon_get_next_evolution_in_sequence(current_code);
            if (next_code) {
                ESP_LOGI(TAG, "Acción: Subir Nivel. De '%s' a '%s'.", current_code, next_code);
                diymon_set_current_code(next_code);
                trigger_ui_reload();
            }
            break;

        case ACTION_ID_LVL_DOWN:
            // Ajustar brillo
            s_current_brightness -= 10;
            if (s_current_brightness < 0) s_current_brightness = 0;
            bsp_display_set_brightness(s_current_brightness);
            ESP_LOGI(TAG, "Acción: Bajar Brillo a %d%%", s_current_brightness);

            // Cambiar nivel
            current_code = diymon_get_current_code();
            next_code = diymon_get_previous_evolution_in_sequence(current_code);
            if (next_code) {
                ESP_LOGI(TAG, "Acción: Bajar Nivel. De '%s' a '%s'.", current_code, next_code);
                diymon_set_current_code(next_code);
                trigger_ui_reload();
            }
            break;
            
        case ACTION_ID_SCREEN_OFF:
            ESP_LOGI(TAG, "Acción: Apagar pantalla.");
            bsp_display_turn_off();
            g_screen_is_off = true;
            break;

        // --- Nuevas acciones del panel lateral ---
        case ACTION_ID_EVO_FIRE:
            ESP_LOGI(TAG, "Acción: Evolucionar a Fuego (lógica pendiente).");
            break;
        case ACTION_ID_EVO_WATER:
            ESP_LOGI(TAG, "Acción: Evolucionar a Agua (lógica pendiente).");
            break;
        case ACTION_ID_EVO_EARTH:
            ESP_LOGI(TAG, "Acción: Evolucionar a Tierra (lógica pendiente).");
            break;
        case ACTION_ID_EVO_WIND:
            ESP_LOGI(TAG, "Acción: Evolucionar a Aire (lógica pendiente).");
            break;
        case ACTION_ID_EVO_BACK:
            ESP_LOGI(TAG, "Acción: Volver de evolución (lógica pendiente).");
            break;

        default:
            ESP_LOGW(TAG, "ID de acción desconocido: %d", action_id);
            break;
    }
}