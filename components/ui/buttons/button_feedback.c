/* Fecha: 18/08/2025 - 09:39  */
/* Fichero: components/ui/buttons/button_feedback.c */
/* Último cambio: Implementación de la lógica de feedback visual. */
/* Descripción: Implementa la lógica de feedback para los botones. Un único callback de evento se encarga de desplazar el icono del botón 2 píxeles en diagonal al ser presionado y de devolverlo a su posición centrada al ser liberado, proporcionando una respuesta táctil visual de bajo coste computacional. */

#include "button_feedback.h"

/**
 * @brief Callback de evento que gestiona el feedback visual del botón.
 *
 * Esta función se dispara con los eventos de presión y liberación.
 * Mueve el primer hijo del objeto (que se asume es el icono) para simular
 * una pulsación física.
 *
 * @param e Puntero al evento de LVGL.
 */
static void feedback_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *icon = lv_obj_get_child(btn, 0); // Asumimos que el icono es el primer hijo

    if (!icon) {
        return; // No hacer nada si no hay un icono que mover
    }

    if (code == LV_EVENT_PRESSED) {
        // Mover el icono 2px hacia abajo y a la derecha
        lv_obj_set_pos(icon, 4, 4);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        // Devolver el icono a su posición centrada original
        lv_obj_center(icon);
    }
}

/**
 * @brief Añade el comportamiento de feedback visual a un botón.
 */
void button_feedback_add(lv_obj_t *btn) {
    if (btn) {
        // Registrar el mismo callback para los tres eventos relevantes
        lv_obj_add_event_cb(btn, feedback_event_cb, LV_EVENT_PRESSED, NULL);
        lv_obj_add_event_cb(btn, feedback_event_cb, LV_EVENT_RELEASED, NULL);
        lv_obj_add_event_cb(btn, feedback_event_cb, LV_EVENT_PRESS_LOST, NULL);
    }
}
