/* Fecha: 16/08/2025 - 06:53  */
/* Fichero: components/diymon_ui/ui_telemetry.h */
/* Último cambio: Añadida declaración de ui_telemetry_force_update() para actualización bajo demanda. */
/* Descripción: Define la interfaz para el módulo de telemetría de la UI, encargado de mostrar datos como el nivel de batería y el código de evolución. */

#ifndef UI_TELEMETRY_H
#define UI_TELEMETRY_H

#include <lvgl.h>

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
 * @brief Fuerza una actualización inmediata de los labels de telemetría.
 *        Útil después de una acción que cambia el estado del Diymon, como una evolución.
 */
void ui_telemetry_force_update(void);

#ifdef __cplusplus
}
#endif

#endif // UI_TELEMETRY_H
