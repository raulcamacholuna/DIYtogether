/*
  Fichero: ./components/diymon_ui/actions.c
  Fecha: 12/08/2025 - 03:30
  Último cambio: Añadida la funcionalidad de borrado de WiFi al botón de brillo.
  Descripción: Lógica para todas las acciones de los botones de la UI. El botón de brillo
               ahora cicla la intensidad y, adicionalmente, borra las credenciales
               WiFi guardadas en NVS, forzando un reinicio al modo portal.
*/
#include "actions.h"
#include "ui_action_animations.h" 
#include "esp_log.h"
#include "diymon_evolution.h"
#include "screen_manager.h" 
#include "wifi_portal.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "DIYMON_ACTIONS";

static int s_brightness_levels[] = {25, 50, 75, 100};
static int s_current_brightness_idx = 3;

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

            ESP_LOGI(TAG, "Borrando credenciales WiFi y reiniciando para entrar en modo portal...");
            wifi_portal_erase_credentials();
            vTaskDelay(pdMS_TO_TICKS(500));
            esp_restart();
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
            
        case ACTION_ID_ENABLE_FTP: {
            ESP_LOGI(TAG, "Accion: Habilitar modo FTP en el proximo reinicio.");
            nvs_handle_t nvs_handle;
            esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
            if (err == ESP_OK) {
                nvs_set_str(nvs_handle, "enable_ftp", "1");
                nvs_commit(nvs_handle);
                nvs_close(nvs_handle);
                ESP_LOGI(TAG, "Marca FTP guardada. Reiniciando en 500ms...");
            } else {
                ESP_LOGE(TAG, "Error (%s) abriendo NVS para habilitar FTP.", esp_err_to_name(err));
            }
            vTaskDelay(pdMS_TO_TICKS(500)); 
            esp_restart();
            break;
        }

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