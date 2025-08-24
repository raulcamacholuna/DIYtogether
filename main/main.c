/* Fichero: main/main.c */
/* Último cambio: 24/08/2025 - 16:06 */
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
#include "zigbee_comm.h"
#include "service_screen.h"

#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "DIYMON_MAIN";

// --- Declaraciones de funciones ---
static void run_main_application_mode(void);
static bool run_game_mode(zigbee_role_t role);
static bool check_and_run_game_mode(void);
static bool verify_sdcard_contents(void);
static void zigbee_data_cb(uint16_t src_addr, game_packet_t* packet);
static void exit_game_mode(void);

void app_main(void) {
    nvs_flash_init();
    
    // Comprobar si debemos arrancar en modo juego. Si es así, esta función no retorna.
    if (check_and_run_game_mode()) {
        // El dispositivo está ahora en un bucle de modo juego.
        // El código aquí abajo no se ejecutará.
        return; 
    }

    // Si check_and_run_game_mode retorna false, procedemos con el arranque normal.
    ESP_LOGI(TAG, "Arrancando en modo aplicación completa.");
    ui_preinit();
    run_main_application_mode();
    
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

static void zigbee_data_cb(uint16_t src_addr, game_packet_t* packet)
{
    ESP_LOGI(TAG, "Paquete Zigbee (Modo Juego) recibido desde 0x%04x", src_addr);
    ESP_LOGI(TAG, "  ID de Juego: %d", packet->game_id);
    ESP_LOGI(TAG, "  Longitud de Payload: %d", packet->len);
    ESP_LOG_BUFFER_HEX(TAG, packet->payload, packet->len);
    
    // TODO: Aquí se procesaría la lógica del minijuego.
    // Por ahora, para salir del modo juego, se puede usar un botón físico
    // o una condición específica del paquete.
}

static void run_main_application_mode(void) {
    ESP_LOGI(TAG, "Cargando aplicación principal...");
    
    hardware_manager_init();
    bool is_sd_ok = verify_sdcard_contents();
    diymon_evolution_init();
    ui_assets_init();

    if (lvgl_port_lock(0)) {
        if (is_sd_ok) {
            hardware_manager_mount_lvgl_filesystem();
        }
        
                ui_init(is_sd_ok);
        state_manager_init();
        lvgl_port_unlock();
    }
    ESP_LOGI(TAG, "Interfaz de Usuario principal inicializada.");

    if (!is_sd_ok) {
        ESP_LOGW(TAG, "Fallo en la SD. Entrando automáticamente en modo de configuración WiFi...");
        vTaskDelay(pdMS_TO_TICKS(500));
        execute_diymon_action(ACTION_ID_ACTIVATE_CONFIG_MODE);
    } else {
        telemetry_task_start();
        ESP_LOGI(TAG, "¡Firmware DIYMON en marcha!");
    }
}

static void exit_game_mode(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_erase_key(nvs_handle, "game_mode");
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        ESP_LOGW(TAG, "Clave 'game_mode' borrada. Reiniciando a modo UI...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Error crítico: no se pudo abrir NVS para salir del modo juego. Reiniciando de todas formas.");
        esp_restart();
    }
}

static bool run_game_mode(zigbee_role_t role) {
    ESP_LOGW(TAG, "Dispositivo arrancando en MODO JUEGO DEDICADO.");
    
    // 1. Inicializar solo el hardware mínimo necesario
    bsp_init_service_mode();
    service_screen_show_from_rom(); // Muestra una pantalla estática
    
    // 2. Inicializar Zigbee con el rol seleccionado
    zigbee_comm_init(role);
    zigbee_comm_register_data_callback(zigbee_data_cb);
    zigbee_comm_start();

    // TODO: Implementar un mecanismo para salir del modo juego
    // Por ejemplo, mantener pulsado un botón físico durante 5 segundos
    // gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
    // gpio_set_pull_mode(GPIO_NUM_0, GPIO_PULLUP_ONLY);

    ESP_LOGW(TAG, "Modo Juego activo. El dispositivo está en un bucle. Reiniciar para salir.");
    while(1) {
        // Bucle principal del modo juego
        // if(gpio_get_level(GPIO_NUM_0) == 0) {
        //     vTaskDelay(pdMS_TO_TICKS(5000));
        //     if(gpio_get_level(GPIO_NUM_0) == 0) {
        //         exit_game_mode();
        //     }
        // }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return true; // Aunque nunca se alcanza, indica que entramos en el modo
}

static bool check_and_run_game_mode(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "NVS no encontrado, arrancando en modo UI normal.");
        return false;
    }

    char game_mode_str[16];
    size_t required_size = sizeof(game_mode_str);
    err = nvs_get_str(nvs_handle, "game_mode", game_mode_str, &required_size);
    nvs_close(nvs_handle);

    if (err == ESP_OK) {
        if (strcmp(game_mode_str, "coordinator") == 0) {
            run_game_mode(ZIGBEE_ROLE_COORDINATOR);
            return true;
        } else if (strcmp(game_mode_str, "router") == 0) {
            run_game_mode(ZIGBEE_ROLE_ROUTER);
            return true;
        }
    }
    
    ESP_LOGI(TAG, "Clave 'game_mode' no encontrada o inválida. Arrancando en modo UI normal.");
    return false;
}
