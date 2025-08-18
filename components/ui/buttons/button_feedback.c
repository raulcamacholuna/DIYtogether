/* Fecha: 18/08/2025 - 10:18  */
/* Fichero: components/ui/buttons/button_feedback.c */
/* Último cambio: Simplificada la lógica para gestionar únicamente el feedback visual en PRESS y RELEASE. */
/* Descripción: Se ha revertido el módulo a su responsabilidad única. El filtrado de gestos se ha movido directamente a los callbacks de acción de cada botón para una mayor fiabilidad, eliminando la lógica de `CLICKED` y de detención de eventos de este fichero central. */

#include "button_feedback.h"

/**
 * @brief Callback de evento que gestiona únicamente el feedback visual del botón.
 *
 * @param e Puntero al evento de LVGL.
 */
static void feedback_visual_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *icon = lv_obj_get_child(btn, 0);

    if (!icon) {
        return;
    }

    if (code == LV_EVENT_PRESSED) {
        // Mover el icono 2px hacia abajo y a la derecha
        lv_obj_set_pos(icon, 2, 2);
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
        // Registrar el callback para los eventos de presión y liberación.
        lv_obj_add_event_cb(btn, feedback_visual_event_cb, LV_EVENT_PRESSED, NULL);
        lv_obj_add_event_cb(btn, feedback_visual_event_cb, LV_EVENT_RELEASED, NULL);
        lv_obj_add_event_cb(btn, feedback_visual_event_cb, LV_EVENT_PRESS_LOST, NULL);
    }
}
