/* Fecha: 17/08/2025 - 01:22 
# Fichero: components\ui\buttons\btn_7.h
# Último cambio: Creación del fichero de cabecera para el botón 'Reset Total' (BTN_7).
# Descripción: Define la interfaz pública para el módulo del botón de reseteo. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_7_H
#define BTN_7_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Reset Total'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_7_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Reset Total'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_7_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_7_H
