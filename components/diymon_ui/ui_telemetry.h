/*
  Fichero: ./components/diymon_ui/ui_telemetry.h
  Fecha: 13/08/2025 - 12:03 
  Último cambio: Creación del fichero.
  Descripción: Interfaz pública para el nuevo módulo de telemetría de la UI.
               Define las funciones para crear y destruir los elementos de la UI
               que mostrarán los datos de los sensores.
*/
#ifndef UI_TELEMETRY_H
#define UI_TELEMETRY_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea los labels y el temporizador para mostrar los datos de los sensores.
 * @param parent El objeto padre sobre el que se crearán los labels.
 */
void ui_telemetry_create(lv_obj_t *parent);

/**
 * @brief Destruye el temporizador de telemetría para liberar recursos.
 */
void ui_telemetry_destroy(void);

#ifdef __cplusplus
}
#endif

#endif // UI_TELEMETRY_H
