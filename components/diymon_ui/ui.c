/*
 * Fichero: ui.c
 * Fecha: 08/08/2025
 * Último cambio: Implementado logger de coordenadas táctiles a través del sistema de eventos de LVGL.
 * Descripción: Punto de entrada principal para el componente de la interfaz de usuario.
 *              Se encarga de inicializar las pantallas y conectar los eventos.
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
 * @brief Callback para registrar las coordenadas de pulsación en pantalla.
 * 
 * Se activa con cualquier evento de pulsación en el objeto al que está asociado.
 * 
 * @param e Puntero al evento de LVGL que contiene el estado de la entrada.
 */
static void touch_logger_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        lv_indev_t *indev = lv_indev_get_act();
        if (indev) {
            lv_point_t point;
            lv_indev_get_point(indev, &point);
            ESP_LOGI(TAG, "Pulsacion en pantalla -> X: %d, Y: %d", point.x, point.y);
        }
    }
}


/**
 * @brief Inicializa y configura toda la interfaz de usuario.
 */
void ui_init() {
    // 1. Crear todos los objetos de la pantalla.
    create_screens();

    // 2. Conectar los botones a sus acciones.
    ui_connect_actions();
    
    // 3. Cargar el fondo dinámico y vincular el logger de pulsaciones.
    if (objects.main) {
        ui_helpers_load_background(objects.main);
        
        // Se registra el callback en la pantalla principal.
        // Se escucha el evento LV_EVENT_PRESSED para capturar cada pulsación.
        lv_obj_add_event_cb(objects.main, touch_logger_event_cb, LV_EVENT_PRESSED, NULL);
        ESP_LOGI(TAG, "Logger de eventos tactiles vinculado a la pantalla principal.");
    }

    // 4. Cargar la pantalla principal para que sea visible.
    lv_screen_load(objects.main);
    
    ESP_LOGI(TAG, "UI dinámica inicializada y lista.");
}