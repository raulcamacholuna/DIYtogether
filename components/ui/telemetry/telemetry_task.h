/* Fichero: components/ui/telemetry/telemetry_task.h */
/* Descripción: Interfaz pública para la tarea de telemetría. Se añade la función 'telemetry_task_stop' para gestionar explícitamente el ciclo de vida de la tarea, permitiendo su detención segura antes de la destrucción de la UI y evitando así condiciones de carrera y fallos de acceso a memoria. */
/* Último cambio: 20/08/2025 - 07:39 */
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

/**
 * @brief Detiene y elimina la tarea de telemetría en segundo plano.
 *
 * Se asegura de que la tarea ya no se ejecute, previniendo que intente
 * acceder a elementos de la UI que están siendo destruidos.
 */
void telemetry_task_stop(void);


#ifdef __cplusplus
}
#endif

#endif // TELEMETRY_TASK_H
