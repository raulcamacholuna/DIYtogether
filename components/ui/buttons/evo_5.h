/* Fecha: 17/08/2025 - 01:28 
# Fichero: components\ui\buttons\evo_5.h
# Último cambio: Creación del fichero de cabecera para el botón 'Involucionar' (EVO_5).
# Descripción: Define la interfaz pública para el módulo del botón de involución. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef EVO_5_H
#define EVO_5_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Involucionar'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void evo_5_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Involucionar'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* evo_5_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // EVO_5_H
