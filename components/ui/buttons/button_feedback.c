/* Fecha: 18/08/2025 - 10:11  */
/* Fichero: components/ui/buttons/button_feedback.c */
/* Último cambio: Añadida la llamada a `lv_event_stop_bubbling` para clics válidos, evitando que el evento se propague a la pantalla principal y cause el cierre del panel. */
/* Descripción: Se ha perfeccionado el filtro de eventos. Ahora, cuando se detecta un clic legítimo (no un swipe), se invoca a `lv_event_stop_bubbling`. Esto evita que el evento de clic se propague hacia la pantalla principal, solucionando el problema por el cual presionar un botón causaba que el panel de acciones se cerrara inesperadamente. */

#include "button_feedback.h"
#include "esp_log.h"

static const char *TAG = "BTN_FEEDBACK";

/**
 * @brief Callback de evento que gestiona el feedback visual y filtra/detiene eventos.
 *
 * @param e Puntero al evento de LVGL.
 */
static void feedback_manager_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *icon = lv_obj_get_child(btn, 0);

    // --- Parte 1: Feedback Visual ---
    if (code == LV_EVENT_PRESSED) {
        if (icon) lv_obj_set_pos(icon, 2, 2);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        if (icon) lv_obj_center(icon);
    } 
    // --- Parte 2: Filtro y Control de Propagación de Clics ---
    else if (code == LV_EVENT_CLICKED) {
        lv_indev_t * indev = lv_indev_get_act();
        if (indev && lv_indev_get_scroll_dir(indev) != LV_DIR_NONE) {
            // Es un swipe, no un clic. Detenemos el procesamiento para este objeto.
            ESP_LOGD(TAG, "Click filtrado (es parte de un swipe).");
            lv_event_stop_processing(e);
        } else {
            // Es un clic válido. Dejamos que la acción del botón se ejecute,
            // pero detenemos el burbujeo para que no llegue a la pantalla.
            ESP_LOGD(TAG, "Click válido. Deteniendo burbujeo.");
            lv_event_stop_bubbling(e);
        }
    }
}

/**
 * @brief Añade el comportamiento de feedback y filtro a un botón.
 */
void button_feedback_add(lv_obj_t *btn) {
    if (btn) {
        // Se registra ANTES que el callback de la acción para poder filtrarlo y detenerlo.
        lv_obj_add_event_cb(btn, feedback_manager_event_cb, LV_EVENT_ALL, NULL);
    }
}
