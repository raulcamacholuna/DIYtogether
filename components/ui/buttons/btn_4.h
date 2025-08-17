/* Fecha: 17/08/2025 - 01:20 
# Fichero: components/ui/buttons/btn_4.h
# Último cambio: Creación del fichero de cabecera para el botón 'Brillo' (BTN_4).
# Descripción: Define la interfaz pública para el módulo del botón 'Brillo'. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_4_H
#define BTN_4_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Brillo'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_4_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Brillo'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_4_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_4_H
