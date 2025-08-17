/* Fecha: 17/08/2025 - 01:20 
# Fichero: components/ui/buttons/btn_5.h
# Último cambio: Creación del fichero de cabecera para el botón 'Apagar Pantalla' (BTN_5).
# Descripción: Define la interfaz pública para el módulo del botón 'Apagar Pantalla'. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador.
*/
#ifndef BTN_5_H
#define BTN_5_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Apagar Pantalla'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_5_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Apagar Pantalla'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_5_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_5_H
