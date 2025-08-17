/* Fecha: 17/08/2025 - 09:43  */
/* Fichero: components/ui/telemetry/telemetry_task.c */
/* Último cambio: Movido a ui/telemetry/ como parte de la refactorización de telemetría. */
/* Descripción: Implementa una tarea de FreeRTOS dedicada a leer el sensor de batería y empujar los datos a la UI. Esta tarea vive en su propio directorio para encapsular toda la lógica de obtención y presentación de datos de sensores. */

#include "telemetry_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_api.h"
#include "telemetry_manager.h"
#include <math.h> // Para fmax y fmin

static const char *TAG = "TELEMETRY_TASK";

// --- Constantes de configuración de la tarea ---
#define TELEMETRY_TASK_STACK_SIZE   4096
#define TELEMETRY_TASK_PRIORITY     5
#define TELEMETRY_TASK_DELAY_MS     200
#define UI_UPDATE_INTERVAL_MS       5000 // Actualizar la UI cada 5 segundos

/**
 * @brief Tarea principal que se ejecuta en segundo plano.
 */
static void telemetry_task_main(void *pvParameters) {
    ESP_LOGI(TAG, "Tarea de telemetría iniciada.");

    uint32_t ui_update_counter = 0;
    const uint32_t ui_update_ticks = UI_UPDATE_INTERVAL_MS / TELEMETRY_TASK_DELAY_MS;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(TELEMETRY_TASK_DELAY_MS));

        // --- Actualización de la UI (periódica) ---
        ui_update_counter++;
        if (ui_update_counter >= ui_update_ticks) {
            ui_update_counter = 0;
            
            // --- Lectura del Sensor de Batería ---
            float voltage;
            uint16_t adc_val;
            bsp_battery_get_voltage(&voltage, &adc_val);
            // Lógica simple para convertir voltaje a porcentaje (ajustar según la curva de la batería)
            uint8_t battery_percentage = (uint8_t)fmax(0.0, fmin(100.0, (voltage - 3.2) / (4.2 - 3.2) * 100.0));

            // Empujar los nuevos valores a la UI para que los muestre
            telemetry_manager_update_values(battery_percentage);
        }
    }
}

/**
 * @brief Inicia la tarea de telemetría.
 */
void telemetry_task_start(void) {
    xTaskCreate(
        telemetry_task_main,
        "telemetry_task",
        TELEMETRY_TASK_STACK_SIZE,
        NULL,
        TELEMETRY_TASK_PRIORITY,
        NULL
    );
}
