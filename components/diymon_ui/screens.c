/*
 * Fichero: ./components/diymon_ui/screens.c
 * Fecha: 10/08/2025 - 03:15
 * Último cambio: Refactorizado para usar los nuevos módulos de UI.
 * Descripción: `create_screen_main` ahora delega la creación de la animación de idle y el panel de acciones a sus respectivos módulos. El callback de eventos se simplifica, manejando solo los toques en la pantalla principal y delegando los gestos al módulo del panel.
 */
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "esp_log.h"

static const char *TAG = "SCREENS";

// Referencias a objetos que otros módulos puedan necesitar
lv_obj_t *g_idle_animation_obj = NULL;
lv_obj_t *g_main_screen_obj = NULL;

static lv_coord_t touch_start_y = -1; // Para la lógica de gestos

// Declaración de la función de callback de eventos
static void main_screen_event_cb(lv_event_t *e);

void create_screen_main(void) {
    g_main_screen_obj = lv_obj_create(NULL);
    lv_obj_clear_flag(g_main_screen_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(g_main_screen_obj, 170, 320);
    lv_obj_add_flag(g_main_screen_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(g_main_screen_obj, main_screen_event_cb, LV_EVENT_ALL, NULL);

    // 1. Iniciar la animación de idle a través de su módulo
    g_idle_animation_obj = ui_idle_animation_start(g_main_screen_obj);

    // 2. Crear las imágenes de las animaciones de acción a través de su módulo
    ui_action_animations_create(g_main_screen_obj);

    // 3. Crear el panel de acciones superior a través de su módulo
    ui_actions_panel_create(g_main_screen_obj);

    ESP_LOGI(TAG, "Pantalla principal creada delegando en módulos.");
}

static void main_screen_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();
    lv_point_t p;

    switch(code) {
        case LV_EVENT_PRESSED:
            lv_indev_get_point(indev, &p);
            ESP_LOGI(TAG, "EVENTO (Pantalla): Pulsado en -> X: %d, Y: %d", p.x, p.y);
            touch_start_y = p.y;
            break;

        case LV_EVENT_RELEASED:
            touch_start_y = -1;
            break;

        case LV_EVENT_GESTURE: {
            lv_dir_t dir = lv_indev_get_gesture_dir(indev);
            ESP_LOGI(TAG, "EVENTO (Pantalla): Gesto detectado.");
            // Delegamos el manejo del gesto al módulo del panel
            ui_actions_panel_handle_gesture(dir, touch_start_y);
            break;
        }
        
        default:
            break;
    }
}

void delete_screen_main(void) {
    if (g_main_screen_obj) {
        // Al borrar la pantalla principal, LVGL borrará todos sus hijos (animación, panel, etc.)
        lv_obj_del(g_main_screen_obj);
        g_main_screen_obj = NULL;
        g_idle_animation_obj = NULL;
    }
    // Detenemos explícitamente los timers y liberamos memoria de los módulos
    ui_idle_animation_stop();
}

void create_screens(void) {
    create_screen_main();
}