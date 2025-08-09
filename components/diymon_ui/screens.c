/*
 * Fichero: ./components/diymon_ui/screens.c
 * Fecha: 10/08/2025 - 04:45
 * Último cambio: Mejorado el logging de eventos de fondo con mayor detalle y color.
 * Descripción: Se actualiza el callback de la pantalla principal para que los logs especifiquen que el evento ocurre en el "Fondo" y se usa `ESP_LOGW` (amarillo) para los gestos, para que destaquen visualmente.
 */
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "esp_log.h"

static const char *TAG = "SCREENS";

lv_obj_t *g_idle_animation_obj = NULL;
lv_obj_t *g_main_screen_obj = NULL;

static lv_coord_t touch_start_y = -1;

static void main_screen_event_cb(lv_event_t *e);

void create_screen_main(void) {
    g_main_screen_obj = lv_obj_create(NULL);
    lv_obj_clear_flag(g_main_screen_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(g_main_screen_obj, 170, 320);
    lv_obj_add_flag(g_main_screen_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(g_main_screen_obj, main_screen_event_cb, LV_EVENT_ALL, NULL);

    g_idle_animation_obj = ui_idle_animation_start(g_main_screen_obj);
    ui_action_animations_create(g_main_screen_obj);
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
            ESP_LOGI(TAG, "EVENTO (Fondo): Pulsado en -> X: %d, Y: %d", p.x, p.y);
            touch_start_y = p.y;
            break;

        case LV_EVENT_PRESSING:
            lv_indev_get_point(indev, &p);
            ESP_LOGI(TAG, "EVENTO (Fondo): Arrastrando en -> X: %d, Y: %d", p.x, p.y);
            break;

        case LV_EVENT_RELEASED:
            lv_indev_get_point(indev, &p);
            ESP_LOGI(TAG, "EVENTO (Fondo): Liberado en -> X: %d, Y: %d", p.x, p.y);
            touch_start_y = -1;
            break;

        case LV_EVENT_GESTURE: {
            lv_dir_t dir = lv_indev_get_gesture_dir(indev);
            const char* dir_str = "Desconocido";
            switch(dir) {
                case LV_DIR_TOP:    dir_str = "ARRIBA"; break;
                case LV_DIR_BOTTOM: dir_str = "ABAJO"; break;
                case LV_DIR_LEFT:   dir_str = "IZQUIERDA"; break;
                case LV_DIR_RIGHT:  dir_str = "DERECHA"; break;
                default: break;
            }
            ESP_LOGW(TAG, "GESTO (Fondo): Arrastre hacia %s", dir_str);
            ui_actions_panel_handle_gesture(dir, touch_start_y);
            break;
        }
        
        default:
            break;
    }
}

void delete_screen_main(void) {
    if (g_main_screen_obj) {
        lv_obj_del(g_main_screen_obj);
        g_main_screen_obj = NULL;
        g_idle_animation_obj = NULL;
    }
    ui_idle_animation_stop();
}

void create_screens(void) {
    create_screen_main();
}