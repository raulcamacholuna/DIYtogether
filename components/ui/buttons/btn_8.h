/* Fecha: 17/08/2025 - 01:24 
# Fichero: components\ui\buttons\btn_8.h
# Último cambio: Creación del fichero de cabecera para el botón 'Servidor de Ficheros' (BTN_8).
# Descripción: Define la interfaz pública para el módulo del botón que activa el servidor de ficheros. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_8_H
#define BTN_8_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Servidor de Ficheros'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_8_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Servidor de Ficheros'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_8_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_8_H
