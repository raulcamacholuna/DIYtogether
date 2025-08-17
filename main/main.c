/* Fecha: 17/08/2025 - 08:01  */
/* Fichero: main/main.c */
/* Último cambio: Simplificada la lógica de arranque para eliminar los modos de servicio obsoletos y unificar el inicio en la aplicación principal. */
/* Descripción: Orquestador principal. Se ha refactorizado 'app_main' para que inicie directamente la aplicación principal. Se eliminaron las comprobaciones de arranque para el portal WiFi (basado en credenciales guardadas o pulsación de botón) y para el modo de servidor de ficheros (basado en flag de NVS). El único modo de configuración ahora es el servidor web accesible desde la UI, eliminando la redundancia y simplificando el flujo de arranque. */

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
#include "core/telemetry_task.h"

#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_main_application_mode(void);
static bool verify_sdcard_contents(void);

void app_main(void) {
    // 1. Inicializar la memoria no volátil.
    nvs_flash_init();

    // 2. Pre-reservar los buffers de memoria más grandes para la UI para evitar fragmentación.
    ui_preinit();

    // 3. Iniciar directamente la aplicación principal.
    //    Se han eliminado todos los modos de arranque alternativos (portal WiFi, servidor de archivos, etc.).
    //    El único modo de configuración es ahora el servidor web, que se activa desde la propia UI.
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
    hardware_manager_init();
    
    bool is_sd_ok = verify_sdcard_contents();

    // El motor de evolución se inicializa independientemente del estado de la SD
    // porque guarda su estado en la NVS interna.
    diymon_evolution_init();
    
    // Los assets de iconos se cargan desde el firmware, por lo que no dependen de la SD.
    ui_assets_init();

    // Inicializa LVGL y la UI.
    if (lvgl_port_lock(0)) {
        // Monta el sistema de ficheros de la SD para que LVGL pueda acceder a las animaciones.
        if (is_sd_ok) {
            hardware_manager_mount_lvgl_filesystem();
        }
        
        ui_init(); // Crea todos los elementos de la UI.
        state_manager_init(); // Inicia el gestor de inactividad de pantalla.
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Interfaz de Usuario principal inicializada.");

    if (!is_sd_ok) {
        // Si la tarjeta SD falla, se notifica al usuario entrando automáticamente
        // al modo de configuración para que pueda subir los archivos.
        ESP_LOGW(TAG, "Fallo en la SD. Entrando automáticamente en modo de configuración WiFi...");
        vTaskDelay(pdMS_TO_TICKS(500));
        execute_diymon_action(ACTION_ID_ACTIVATE_CONFIG_MODE);
    } else {
        // Si todo está bien, inicia la tarea de telemetría en segundo plano.
        telemetry_task_start();
        ESP_LOGI(TAG, "¡Firmware DIYMON en marcha!");
    }
}
