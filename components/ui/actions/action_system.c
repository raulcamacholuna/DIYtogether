/* Fecha: 18/08/2025 - 07:00  */
/* Fichero: components/ui/actions/action_system.c */
/* Último cambio: Eliminada la inclusión de 'wifi_portal.h' para resolver el error de compilación. */
/* Descripción: Se ha eliminado la directiva '#include "wifi_portal.h"' porque el componente 'wifi_portal' fue borrado del proyecto. Las funciones para borrar credenciales ahora residen en el BSP o en un nuevo módulo de gestión de NVS, pero por ahora se elimina la dependencia directa para permitir la compilación. */

#include "actions/action_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include "bsp_api.h" // Se asume que el borrado de credenciales estará aquí o en un gestor NVS

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
 * @brief Función interna para borrar credenciales WiFi.
 * @note Esto es una solución temporal. Idealmente, esta lógica debería estar
 *       en un módulo dedicado a la gestión de NVS o en el BSP.
 */
static void erase_wifi_credentials_from_nvs(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) return;
    nvs_erase_key(nvs_handle, "wifi_ssid");
    nvs_erase_key(nvs_handle, "wifi_pass");
    nvs_erase_key(nvs_handle, "wifi_authmode");
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Credenciales WiFi borradas de NVS.");
}


/**
 * @brief Implementación del reseteo total del dispositivo.
 */
void action_system_reset_all(void) {
    ESP_LOGW(TAG, "Ejecutando reseteo total del dispositivo...");
    
    // Borra todas las configuraciones guardadas
    erase_wifi_credentials_from_nvs();
    diymon_evolution_reset_state();
    erase_nvs_key("file_server"); // Se mantiene por si quedan restos de versiones anteriores

    ESP_LOGW(TAG, "Todas las configuraciones borradas. Reiniciando ahora.");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}
