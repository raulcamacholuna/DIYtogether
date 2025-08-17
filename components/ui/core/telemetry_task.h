/* Fecha: 17/08/2025 - 02:31  */
/* Fichero: components/ui/core/telemetry_task.h */
/* Último cambio: Movido a ui/core/ como parte de la refactorización. */
/* Descripción: Interfaz pública para la tarea de telemetría. Esta tarea se ejecuta en segundo plano para leer sensores, actualizar la UI y gestionar la lógica de 'shake-to-wake'. */

#ifndef TELEMETRY_TASK_H
#define TELEMETRY_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicia la tarea de telemetría en segundo plano.
 *
 * Crea y lanza una tarea de FreeRTOS que se encargará de leer periódicamente
 * los sensores del dispositivo (batería, IMU) y de actualizar la UI con
 * esta información. También implementa la lógica de 'shake-to-wake'.
 */
void telemetry_task_start(void);

#ifdef __cplusplus
}
#endif

#endif // TELEMETRY_TASK_H
