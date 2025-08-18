/* Fecha: 18/08/2025 - 08:25  */
/* Fichero: main/main.c */
/* Último cambio: Eliminada la inicialización y conexión automática de WiFi en el arranque. */
/* Descripción: Se ha eliminado toda la lógica de inicialización de WiFi del arranque principal (app_main). Ahora, el dispositivo arranca sin activar la red. La responsabilidad de gestionar el ciclo de vida de WiFi (activar, conectar, desactivar) se delega completamente al módulo 'action_config_mode', que se invoca por interacción del usuario. */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "esp_lvgl_port.h"
#include "sys/stat.h"
#include <dirent.h>
#include "driver/gpio.h"

#include "bsp_api.h"
#include "hardware_manager.h"
#include "diymon_evolution.h"
#include "core/ui.h"
#include "web_server.h"
#include "screen_manager.h"
#include "ui_asset_loader.h" 
#include "actions.h"
#include "core/state_manager.h"
#include "telemetry/telemetry_task.h"

#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_main_application_mode(void);
static bool verify_sdcard_contents(void);

void app_main(void) {
    // 1. Inicializar la memoria no volátil.
    nvs_flash_init();

    // [CORRECCIÓN] Se elimina la inicialización de WiFi del arranque.
    // La red solo se activará bajo demanda desde el menú de configuración.
    ESP_LOGI(TAG, "Arrancando en modo offline. WiFi se activará bajo demanda.");

    // 2. Pre-reservar los buffers de memoria más grandes para la UI para evitar fragmentación.
    ui_preinit();

    // 3. El sistema ahora arranca directamente en el modo de aplicación principal.
    run_main_application_mode();
    
    // Este bucle no debería alcanzarse, pero es una buena práctica tenerlo.
    while (1) { 
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

static bool verify_sdcard_contents(void) {
    const char* dir_path = "/sdcard/diymon";
    DIR* dir = opendir(dir_path);
    if (!dir) {
        ESP_LOGE(TAG, "¡ERROR! No se pudo abrir el directorio '%s'. La funcionalidad de animación estará deshabilitada.", dir_path);
        return false;
    }
    closedir(dir);
    ESP_LOGI(TAG, "Verificación de la tarjeta SD completada. Directorio 'diymon' encontrado.");
    return true;
}

static void run_main_application_mode(void) {
    ESP_LOGI(TAG, "Cargando aplicación principal...");
    
    // 1. Inicializa todo el hardware y LVGL.
    hardware_manager_init();
    
    // 2. Verifica la tarjeta SD.
    bool is_sd_ok = verify_sdcard_contents();

    // 3. Inicializa los sistemas de software (Evolución y Assets).
    diymon_evolution_init();
    ui_assets_init();

    // 4. Construye la UI completa.
    if (lvgl_port_lock(0)) {
        if (is_sd_ok) {
            hardware_manager_mount_lvgl_filesystem();
        }
        
        ui_init(); // Crea todos los elementos.
        state_manager_init(); // Inicia el gestor de inactividad.
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Interfaz de Usuario principal inicializada.");

    // 5. Decide el siguiente paso basado en el estado de la SD.
    if (!is_sd_ok) {
        // Si la SD falla, se invoca la acción de modo configuración.
        ESP_LOGW(TAG, "Fallo en la SD. Entrando automáticamente en modo de configuración WiFi...");
        vTaskDelay(pdMS_TO_TICKS(500));
        execute_diymon_action(ACTION_ID_ACTIVATE_CONFIG_MODE);
    } else {
        // Si todo está bien, inicia la tarea de telemetría en segundo plano.
        telemetry_task_start();
        ESP_LOGI(TAG, "¡Firmware DIYMON en marcha!");
    }
}
