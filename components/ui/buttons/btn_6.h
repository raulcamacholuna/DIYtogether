/* Fecha: 17/08/2025 - 01:21 
# Fichero: components\ui\buttons\btn_6.h
# Último cambio: Creación del fichero de cabecera para el botón 'Modo Config' (BTN_6).
# Descripción: Define la interfaz pública para el módulo del botón que activa el modo de configuración. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_6_H
#define BTN_6_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Modo Config'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_6_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Modo Config'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_6_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_6_H
