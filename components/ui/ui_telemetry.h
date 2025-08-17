/* Fecha: 17/08/2025 - 02:14  */
/* Fichero: components/ui/ui_telemetry.h */
/* Último cambio: Refactorizado para recibir datos de la telemetry_task en lugar de usar un temporizador propio. */
/* Descripción: Interfaz del módulo de telemetría de la UI. Se ha eliminado la lógica de temporizador interna y ahora expone una función ui_telemetry_update_values para que una tarea externa (telemetry_task) pueda empujar los datos del sensor para su visualización. */

#ifndef UI_TELEMETRY_H
#define UI_TELEMETRY_H

#include <lvgl.h>
#include <stdint.h> // for uint8_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea y configura los elementos de la UI para la telemetría.
 * 
 * @param parent El objeto padre sobre el cual se crearán los labels.
 */
void ui_telemetry_create(lv_obj_t *parent);

/**
 * @brief Destruye los recursos asociados al módulo de telemetría.
 */
void ui_telemetry_destroy(void);

/**
 * @brief Actualiza los valores mostrados en los labels de telemetría.
 *        Esta función es segura para ser llamada desde otras tareas (thread-safe).
 * 
 * @param battery_percentage El nivel de batería actual en porcentaje (0-100).
 */
void ui_telemetry_update_values(uint8_t battery_percentage);

#ifdef __cplusplus
}
#endif

#endif // UI_TELEMETRY_H
