/*
 * Fichero: ./components/diymon_ui/actions.c
 * Fecha: 11/08/2025 - 16:15
 * Último cambio: Versión final y verificada.
 * Descripción: Lógica para todas las acciones de los botones de la UI.
 */
#include "actions.h"
#include "ui_action_animations.h" 
#include "esp_log.h"
#include "diymon_evolution.h"
#include "ui.h"
#include "screens.h"
#include "screen_manager.h" 
#include "wifi_portal.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "DIYMON_ACTIONS";

// Variables para el ciclado de brillo
static int s_brightness_levels[] = {25, 50, 75, 100};
static int s_current_brightness_idx = 3; // Índice inicial para 100%

void execute_diymon_action(diymon_action_id_t action_id, lv_obj_t* idle_obj) {

    switch(action_id) {
        case ACTION_ID_COMER:
        case ACTION_ID_EJERCICIO:
        case ACTION_ID_ATACAR:
            ESP_LOGI(TAG, "Accion de jugador. Delegando a reproductor.");
            ui_action_animations_play(action_id, idle_obj);
            break;

        case ACTION_ID_BRIGHTNESS_CYCLE:
            s_current_brightness_idx = (s_current_brightness_idx + 1) % (sizeof(s_brightness_levels) / sizeof(s_brightness_levels[0]));
            int new_brightness = s_brightness_levels[s_current_brightness_idx];
            screen_manager_set_brightness(new_brightness);
            ESP_LOGI(TAG, "Accion: Ciclar Brillo a %d%%", new_brightness);
            break;

        case ACTION_ID_TOGGLE_SCREEN:
            if (screen_manager_is_off()) {
                ESP_LOGI(TAG, "Accion: Encender pantalla.");
                screen_manager_turn_on();
            } else {
                ESP_LOGI(TAG, "Accion: Apagar pantalla.");
                screen_manager_turn_off();
            }
            break;
            
        case ACTION_ID_ERASE_WIFI:
            ESP_LOGI(TAG, "Accion: Borrar configuracion WiFi y reiniciar.");
            wifi_portal_erase_credentials();
            vTaskDelay(pdMS_TO_TICKS(500)); 
            esp_restart();
            break;

        // --- Acciones del panel lateral de evolución ---
        case ACTION_ID_EVO_FIRE:
        case ACTION_ID_EVO_WATER:
        case ACTION_ID_EVO_EARTH:
        case ACTION_ID_EVO_WIND:
        case ACTION_ID_EVO_BACK:
            ESP_LOGI(TAG, "Accion de evolucion (logica pendiente).");
            break;

        default:
            ESP_LOGW(TAG, "ID de accion desconocido: %d", action_id);
            break;
    }
}