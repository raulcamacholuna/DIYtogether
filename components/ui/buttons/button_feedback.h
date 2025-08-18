/* Fecha: 18/08/2025 - 09:34  */
/* Fichero: components/ui/buttons/button_feedback.h */
/* Último cambio: Creación del módulo de feedback visual para botones. */
/* Descripción: Interfaz pública del nuevo módulo de feedback. Proporciona una única función para añadir una respuesta visual estándar (un ligero desplazamiento) a cualquier objeto LVGL clicable, mejorando la experiencia de usuario de forma centralizada y de bajo coste. */

#ifndef BUTTON_FEEDBACK_H
#define BUTTON_FEEDBACK_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Añade un comportamiento de feedback visual a un botón u objeto clicable.
 *
 * Registra los callbacks de eventos necesarios para que el objeto se desplace
 * ligeramente hacia abajo al ser presionado y vuelva a su posición original
 * al ser liberado, proporcionando una respuesta visual simple y de bajo coste.
 *
 * @param btn Puntero al objeto lv_obj_t al que se le aplicará el feedback.
 */
void button_feedback_add(lv_obj_t *btn);

#ifdef __cplusplus
}
#endif

#endif // BUTTON_FEEDBACK_H
