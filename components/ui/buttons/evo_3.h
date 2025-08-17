/* Fecha: 17/08/2025 - 01:26 
# Fichero: components\ui\buttons\evo_3.h
# Último cambio: Creación del fichero de cabecera para el botón 'Evo Tierra' (EVO_3).
# Descripción: Define la interfaz pública para el módulo del botón de evolución de Tierra. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef EVO_3_H
#define EVO_3_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Evo Tierra'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void evo_3_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Evo Tierra'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* evo_3_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // EVO_3_H
