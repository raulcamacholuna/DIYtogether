/*
 * Fichero: ./components/diymon_ui/ui.c
 * Fecha: 08/08/2025 - 16:45
 * Último cambio: Restaurado el logger de eventos táctiles detallado para depuración.
 * Descripción: Se reintroduce el callback de depuración táctil en la pantalla principal para registrar la secuencia completa de interacciones (Pressed, Dragging, Released, Gesture), facilitando el diagnóstico de problemas de gestos.
 */

#include "ui.h"
#include "ui_priv.h"
#include "screens.h"
#include "actions.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "DIYMON_UI";

/**
 * @brief Conecta los objetos de la UI (botones) con sus correspondientes acciones.
 */
static void ui_connect_actions() {
    ESP_LOGI(TAG, "Conectando eventos de botones al sistema de acciones...");
    
    if (objects.comer) {
        lv_obj_add_event_cb(objects.comer, execute_diymon_action, LV_EVENT_CLICKED, (void*)(intptr_t)ACTION_ID_COMER);
    }
    if (objects.pesas) {
        lv_obj_add_event_cb(objects.pesas, execute_diymon_action, LV_EVENT_CLICKED, (void*)(intptr_t)ACTION_ID_EJERCICIO);
    }
    if (objects.atacar) {
        lv_obj_add_event_cb(objects.atacar, execute_diymon_action, LV_EVENT_CLICKED, (void*)(intptr_t)ACTION_ID_ATACAR);
    }
}

/**
 * @brief Callback de diagnóstico para registrar todos los eventos de entrada táctil.
 * 
 * Captura y muestra en el log la secuencia completa de una interacción táctil:
 * PRESSED, DRAGGING, RELEASED y GESTURE, para facilitar la depuración.
 * 
 * @param e Puntero al evento de LVGL que contiene el estado de la entrada.
 */
static void touch_logger_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();
    if (!indev) return;

    lv_point_t point;
    lv_indev_get_point(indev, &point);

    switch (code) {
        case LV_EVENT_PRESSED:
            ESP_LOGI(TAG, "Touch Logger -> PRESSED at X: %d, Y: %d", point.x, point.y);
            break;
        case LV_EVENT_PRESSING:
            ESP_LOGI(TAG, "Touch Logger -> DRAGGING to X: %d, Y: %d", point.x, point.y);
            break;
        case LV_EVENT_RELEASED:
            ESP_LOGI(TAG, "Touch Logger -> RELEASED at X: %d, Y: %d", point.x, point.y);
            break;
        case LV_EVENT_GESTURE: {
            lv_dir_t dir = lv_indev_get_gesture_dir(indev);
            const char* dir_str = (dir == LV_DIR_TOP) ? "UP" : (dir == LV_DIR_BOTTOM) ? "DOWN" : (dir == LV_DIR_LEFT) ? "LEFT" : "RIGHT";
            ESP_LOGW(TAG, "Touch Logger -> GESTURE DETECTED! Direction: %s", dir_str);
            break;
        }
        default:
            break;
    }
}

/**
 * @brief Inicializa y configura toda la interfaz de usuario.
 */
void ui_init() {
    create_screens();
    ui_connect_actions();
    
    if (objects.main) {
        ui_helpers_load_background(objects.main);
        // Se añade el logger de diagnóstico a la pantalla principal para capturar todos los eventos.
        lv_obj_add_event_cb(objects.main, touch_logger_event_cb, LV_EVENT_ALL, NULL);
        ESP_LOGI(TAG, "Logger de eventos táctiles detallado VINCULADO a la pantalla principal.");
    }

    lv_screen_load(objects.main);
    ESP_LOGI(TAG, "UI dinámica inicializada y lista.");
}