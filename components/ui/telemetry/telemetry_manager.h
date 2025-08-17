/* Fecha: 17/08/2025 - 09:41  */
/* Fichero: components/ui/telemetry/telemetry_manager.h */
/* Último cambio: Movido a ui/telemetry/ como parte de la refactorización de telemetría. */
/* Descripción: Interfaz del módulo de telemetría de la UI. Expone funciones para crear, destruir y actualizar los elementos visuales de telemetría (batería, EVO). */

#ifndef TELEMETRY_MANAGER_H
#define TELEMETRY_MANAGER_H

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
void telemetry_manager_create(lv_obj_t *parent);

/**
 * @brief Destruye los recursos asociados al módulo de telemetría.
 */
void telemetry_manager_destroy(void);

/**
 * @brief Actualiza los valores mostrados en los labels de telemetría.
 *        Esta función es segura para ser llamada desde otras tareas (thread-safe).
 * 
 * @param battery_percentage El nivel de batería actual en porcentaje (0-100).
 */
void telemetry_manager_update_values(uint8_t battery_percentage);

#ifdef __cplusplus
}
#endif

#endif // TELEMETRY_MANAGER_H
