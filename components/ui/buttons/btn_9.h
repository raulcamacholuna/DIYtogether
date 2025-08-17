/* Fecha: 17/08/2025 - 01:24 
# Fichero: components\ui\buttons\btn_9.h
# Último cambio: Creación del fichero de cabecera para el botón 'Placeholder de Config' (BTN_9).
# Descripción: Define la interfaz pública para el módulo del botón placeholder del panel de configuración. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_9_H
#define BTN_9_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Placeholder de Config'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_9_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Placeholder de Config'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_9_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_9_H
