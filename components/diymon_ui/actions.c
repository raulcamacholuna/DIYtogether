/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\actions.c
# Fecha: `$timestamp
# Último cambio: Acortada la clave NVS a 'file_server' para evitar error de longitud.
# Descripción: Orquestador de acciones de la UI. Gestiona la lógica que se ejecuta al interactuar con los botones, como cambiar el brillo, evolucionar la mascota o activar modos especiales.
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

static void erase_nvs_key(const char* key) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_erase_key(nvs_handle, key);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

void execute_diymon_action(diymon_action_id_t action_id) {
    switch(action_id) {
        case ACTION_ID_COMER:
        case ACTION_ID_EJERCICIO:
        case ACTION_ID_ATACAR:
            ESP_LOGI(TAG, "Accion de jugador. Delegando a reproductor.");
            ui_action_animations_play(action_id);
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

        case ACTION_ID_RESET_ALL:
            ESP_LOGW(TAG, "ACCIÓN: Borrado completo de configuraciones.");
            wifi_portal_erase_credentials();
            diymon_evolution_reset_state();
            erase_nvs_key("file_server");
            ESP_LOGW(TAG, "Todas las configuraciones han sido borradas. Reiniciando en 1 segundo...");
            vTaskDelay(pdMS_TO_TICKS(1000));
            esp_restart();
            break;
            
        case ACTION_ID_ENABLE_FILE_SERVER: {
            ESP_LOGI(TAG, "Accion: Habilitar modo Servidor de Archivos en el proximo reinicio.");
            nvs_handle_t nvs_handle;
            esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
            if (err == ESP_OK) {
                err = nvs_set_str(nvs_handle, "file_server", "1");
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Error (%s) al establecer la bandera 'file_server' en NVS.", esp_err_to_name(err));
                }

                err = nvs_commit(nvs_handle);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Error (%s) al hacer commit de la bandera 'file_server' en NVS.", esp_err_to_name(err));
                } else {
                    ESP_LOGI(TAG, "Marca de modo servidor de archivos guardada y commit realizado. Reiniciando en 2 segundos...");
                }
                
                nvs_close(nvs_handle);
            } else {
                ESP_LOGE(TAG, "Error (%s) abriendo NVS para habilitar modo servidor de archivos.", esp_err_to_name(err));
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
            esp_restart();
            break;
        }

        case ACTION_ID_EVO_FIRE:
        case ACTION_ID_EVO_WATER:
        case ACTION_ID_EVO_EARTH:
        case ACTION_ID_EVO_WIND: {
            const char* current_code = diymon_get_current_code();
            int branch_id = 0;
            if (action_id == ACTION_ID_EVO_FIRE)  branch_id = 1;
            if (action_id == ACTION_ID_EVO_WATER) branch_id = 2;
            if (action_id == ACTION_ID_EVO_EARTH) branch_id = 3;
            if (action_id == ACTION_ID_EVO_WIND)  branch_id = 4;
            
            const char* next_code = diymon_get_branched_evolution(current_code, branch_id);
            if (next_code) {
                ESP_LOGI(TAG, "Evolucionando de '%s' a '%s'. Reiniciando...", current_code, next_code);
                diymon_set_current_code(next_code);
                vTaskDelay(pdMS_TO_TICKS(500));
                esp_restart();
            } else {
                ESP_LOGW(TAG, "Evolución no válida desde '%s' con la rama %d.", current_code, branch_id);
            }
            break;
        }

        case ACTION_ID_EVO_BACK: {
            const char* current_code = diymon_get_current_code();
            const char* prev_code = diymon_get_previous_evolution_in_sequence(current_code);
            if (prev_code) {
                ESP_LOGI(TAG, "Involucionando de '%s' a '%s'. Reiniciando...", current_code, prev_code);
                diymon_set_current_code(prev_code);
                vTaskDelay(pdMS_TO_TICKS(500));
                esp_restart();
            } else {
                ESP_LOGW(TAG, "Ya se encuentra en la forma base '%s'. No se puede involucionar.", current_code);
            }
            break;
        }
        
        case ACTION_ID_ADMIN_PLACEHOLDER:
        case ACTION_ID_CONFIG_PLACEHOLDER:
            ESP_LOGI(TAG, "Accion %d (sin implementación actual).", action_id);
            break;

        default:
            ESP_LOGW(TAG, "ID de accion desconocido: %d", action_id);
            break;
    }
}
