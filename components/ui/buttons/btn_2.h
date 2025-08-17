/* Fecha: 17/08/2025 - 01:18 
# Fichero: components/ui/buttons/btn_2.h
# Último cambio: Creación del fichero de cabecera para el botón 'Ejercicio' (BTN_2).
# Descripción: Define la interfaz pública para el módulo del botón 'Ejercicio'. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_2_H
#define BTN_2_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Ejercicio'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_2_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Ejercicio'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_2_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_2_H
