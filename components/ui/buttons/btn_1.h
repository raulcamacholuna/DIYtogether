/* Fecha: 17/08/2025 - 01:17 
# Fichero: components/ui/buttons/btn_1.h
# Último cambio: Creación del fichero de cabecera para el botón 'Comer' (BTN_1).
# Descripción: Define la interfaz pública para el módulo del botón 'Comer'. Expone funciones para crear el objeto LVGL del botón y para obtener su manejador, encapsulando su lógica.
*/
#ifndef BTN_1_H
#define BTN_1_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea el objeto LVGL para el botón 'Comer'.
 * 
 * @param parent El objeto LVGL padre sobre el cual se creará el botón.
 */
void btn_1_create(lv_obj_t *parent);

/**
 * @brief Obtiene el manejador del botón 'Comer'.
 * 
 * @return Un puntero al objeto lv_obj_t del botón.
 */
lv_obj_t* btn_1_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // BTN_1_H
