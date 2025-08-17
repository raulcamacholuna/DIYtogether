/* Fecha: 17/08/2025 - 02:13  */
/* Fichero: components/ui/telemetry_task.c */
/* Último cambio: Creación de la tarea de telemetría para desacoplar la lectura de sensores de la UI. */
/* Descripción: Implementa una tarea de FreeRTOS dedicada a leer sensores en segundo plano. Esta tarea gestiona la lectura periódica de la batería y el IMU, implementa la lógica de 'shake-to-wake' para despertar la pantalla, y empuja los datos actualizados a la UI. Esto mejora la reactividad y modularidad del sistema. */

#include "telemetry_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_api.h"
#include "screen_manager.h"
#include "ui_telemetry.h"
#include <math.h> // Para sqrtf

static const char *TAG = "TELEMETRY_TASK";

// --- Constantes de configuración de la tarea ---
#define TELEMETRY_TASK_STACK_SIZE   4096
#define TELEMETRY_TASK_PRIORITY     5
#define TELEMETRY_TASK_DELAY_MS     200
#define SHAKE_THRESHOLD_G           1.5f // Umbral de agitación en G's
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

        // --- Lectura de Sensores ---
        float acc[3], gyro[3];
        bsp_imu_read(acc, gyro);

        float voltage;
        uint16_t adc_val;
        bsp_battery_get_voltage(&voltage, &adc_val);
        // Lógica simple para convertir voltaje a porcentaje (ajustar según la curva de la batería)
        uint8_t battery_percentage = (uint8_t)fmax(0.0, fmin(100.0, (voltage - 3.2) / (4.2 - 3.2) * 100.0));


        // --- Lógica de Shake-to-Wake ---
        float acceleration_magnitude = sqrtf(acc[0] * acc[0] + acc[1] * acc[1] + acc[2] * acc[2]) / 9.81f;
        if (acceleration_magnitude > SHAKE_THRESHOLD_G) {
            if (screen_manager_is_off()) {
                ESP_LOGI(TAG, "¡Agitación detectada! (Magnitud: %.2f G). Despertando pantalla.", acceleration_magnitude);
                screen_manager_turn_on();
            }
        }
        
        // --- Actualización de la UI (periódica) ---
        ui_update_counter++;
        if (ui_update_counter >= ui_update_ticks) {
            ui_update_counter = 0;
            // Empujar los nuevos valores a la UI para que los muestre
            ui_telemetry_update_values(battery_percentage);
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
