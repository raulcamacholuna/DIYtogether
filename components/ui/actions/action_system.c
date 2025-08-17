/* Fecha: 17/08/2025 - 01:51  */
/* Fichero: components/ui/actions/action_system.c */
/* Último cambio: Implementación de las acciones de sistema (servidor de ficheros, reset). */
/* Descripción: Módulo que implementa acciones destructivas o que cambian el modo de operación del dispositivo. Interactúa con NVS para establecer banderas o borrar configuraciones y fuerza reinicios del sistema. */

#include "actions/action_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_portal.h"
#include "diymon_evolution.h"
#include "esp_log.h"

static const char *TAG = "ACTION_SYSTEM";

/**
 * @brief Función de ayuda para borrar una clave específica de la NVS.
 */
static void erase_nvs_key(const char* key) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_erase_key(nvs_handle, key);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        ESP_LOGI(TAG, "Clave NVS '%s' borrada.", key);
    } else {
        ESP_LOGE(TAG, "Error al abrir NVS para borrar la clave '%s'.", key);
    }
}

/**
 * @brief Implementación de la activación del modo servidor de ficheros.
 */
void action_system_enable_file_server(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_set_str(nvs_handle, "file_server", "1");
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        ESP_LOGI(TAG, "Bandera 'file_server' establecida. Reiniciando en modo servidor de ficheros...");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Error al abrir NVS para establecer la bandera 'file_server'.");
    }
}

/**
 * @brief Implementación del reseteo total del dispositivo.
 */
void action_system_reset_all(void) {
    ESP_LOGW(TAG, "Ejecutando reseteo total del dispositivo...");
    
    // Borra todas las configuraciones guardadas
    wifi_portal_erase_credentials();
    diymon_evolution_reset_state();
    erase_nvs_key("file_server");

    ESP_LOGW(TAG, "Todas las configuraciones borradas. Reiniciando ahora.");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}
