/* Fecha: 17/08/2025 - 09:42  */
/* Fichero: components/ui/telemetry/telemetry_task.h */
/* Último cambio: Movido a ui/telemetry/ como parte de la refactorización de telemetría. */
/* Descripción: Interfaz pública para la tarea de telemetría. Esta tarea se ejecuta en segundo plano para leer periódicamente el estado de la batería y actualizar la UI. */

#ifndef TELEMETRY_TASK_H
#define TELEMETRY_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicia la tarea de telemetría en segundo plano.
 *
 * Crea y lanza una tarea de FreeRTOS que se encargará de leer periódicamente
 * el estado de la batería del dispositivo y de actualizar la UI con
 * esta información.
 */
void telemetry_task_start(void);

#ifdef __cplusplus
}
#endif

#endif // TELEMETRY_TASK_H
