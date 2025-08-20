/* Fichero: components/ui/telemetry/telemetry_task.c */
/* Descripción: Se implementa la gestión explícita del ciclo de vida de la tarea. La función 'telemetry_task_start' ahora almacena el manejador de la tarea creada, y se introduce 'telemetry_task_stop' para eliminarla de forma segura. Esto resuelve el 'Load access fault' al garantizar que la tarea de telemetría se detiene antes de que se destruyan los objetos de la UI que intenta actualizar. */
/* Último cambio: 20/08/2025 - 07:39 */
#include "telemetry_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_api.h"
#include "telemetry_manager.h"
#include <math.h> // Para fmax y fmin
#include "esp_wifi.h" // Necesario para comprobar el modo WiFi

static const char *TAG = "TELEMETRY_TASK";

// --- Constantes de configuración de la tarea ---
#define TELEMETRY_TASK_STACK_SIZE   4096
#define TELEMETRY_TASK_PRIORITY     5
#define TELEMETRY_TASK_DELAY_MS     200
#define UI_UPDATE_INTERVAL_MS       5000 // Actualizar la UI cada 5 segundos

static TaskHandle_t s_telemetry_task_handle = NULL;

/**
 * @brief Tarea principal que se ejecuta en segundo plano.
 */
static void telemetry_task_main(void *pvParameters) {
    ESP_LOGI(TAG, "Tarea de telemetría iniciada.");

    uint32_t ui_update_counter = 0;
    const uint32_t ui_update_ticks = UI_UPDATE_INTERVAL_MS / TELEMETRY_TASK_DELAY_MS;

    while (1) {
        wifi_mode_t current_mode;
        if (esp_wifi_get_mode(&current_mode) == ESP_OK && current_mode == WIFI_MODE_AP) {
            vTaskDelay(pdMS_TO_TICKS(1000)); 
            continue; 
        }

        vTaskDelay(pdMS_TO_TICKS(TELEMETRY_TASK_DELAY_MS));

        ui_update_counter++;
        if (ui_update_counter >= ui_update_ticks) {
            ui_update_counter = 0;
            
            float voltage;
            uint16_t adc_val;
            bsp_battery_get_voltage(&voltage, &adc_val);
            uint8_t battery_percentage = (uint8_t)fmax(0.0, fmin(100.0, (voltage - 3.2) / (4.2 - 3.2) * 100.0));

            telemetry_manager_update_values(battery_percentage);
        }
    }
}

/**
 * @brief Inicia la tarea de telemetría.
 */
void telemetry_task_start(void) {
    if (s_telemetry_task_handle == NULL) {
        xTaskCreate(
            telemetry_task_main,
            "telemetry_task",
            TELEMETRY_TASK_STACK_SIZE,
            NULL,
            TELEMETRY_TASK_PRIORITY,
            &s_telemetry_task_handle
        );
    }
}

/**
 * @brief Detiene y elimina la tarea de telemetría.
 */
void telemetry_task_stop(void) {
    if (s_telemetry_task_handle != NULL) {
        vTaskDelete(s_telemetry_task_handle);
        s_telemetry_task_handle = NULL;
        ESP_LOGI(TAG, "Tarea de telemetría detenida y eliminada.");
    }
}
