/* Fecha: 17/08/2025 - 01:19 
# Fichero: components/ui/buttons/btn_3.h
# Último cambio: Creación del fichero de cabecera para el botón 'Atacar' (BTN_3).
# Descripción: Define la interfaz pública para el módulo del botón 'Atacar'. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_3_H
#define BTN_3_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Atacar'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_3_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Atacar'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_3_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_3_H
