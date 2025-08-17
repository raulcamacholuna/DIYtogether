/* Fecha: 17/08/2025 - 01:25 
# Fichero: components\ui\buttons\evo_1.h
# Último cambio: Creación del fichero de cabecera para el botón 'Evo Fuego' (EVO_1).
# Descripción: Define la interfaz pública para el módulo del botón de evolución de Fuego. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef EVO_1_H
#define EVO_1_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Evo Fuego'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void evo_1_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Evo Fuego'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* evo_1_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // EVO_1_H
