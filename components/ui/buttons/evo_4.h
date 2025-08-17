/* Fecha: 17/08/2025 - 01:27 
# Fichero: components\ui\buttons\evo_4.h
# Último cambio: Creación del fichero de cabecera para el botón 'Evo Viento' (EVO_4).
# Descripción: Define la interfaz pública para el módulo del botón de evolución de Viento. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef EVO_4_H
#define EVO_4_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Evo Viento'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void evo_4_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Evo Viento'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* evo_4_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // EVO_4_H
