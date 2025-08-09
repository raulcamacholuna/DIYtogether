/*
 * Fichero: ./components/diymon_ui/screens.c
 * Fecha: 10/08/2025 - 22:00
 * Último cambio: Corregido el orden de inicialización de los módulos de animación.
 * Descripción: Se ha cambiado el orden en `create_screen_main` para que el
 *              búfer de animación compartido se cree *antes* de que la animación
 *              de reposo intente usarlo, solucionando el pánico del sistema.
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

    // --- SOLUCIÓN: Corregido el orden de inicialización ---
    // 1. Crear el gestor de animaciones de acción, que reserva el búfer compartido.
    ui_action_animations_create(g_main_screen_obj);
    // 2. Iniciar la animación de idle, que AHORA puede usar el búfer ya creado.
    g_idle_animation_obj = ui_idle_animation_start(g_main_screen_obj);
    // 3. Crear el panel de acciones.
    ui_actions_panel_create(g_main_screen_obj);

    ESP_LOGI(TAG, "Pantalla principal creada delegando en módulos (orden corregido).");
}

static void main_screen_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();

    switch(code) {
        case LV_EVENT_PRESSED: {
            lv_point_t p;
            lv_indev_get_point(indev, &p);
            touch_start_y = p.y;
            break;
        }
        case LV_EVENT_RELEASED:
            touch_start_y = -1;
            break;
        case LV_EVENT_GESTURE: {
            lv_dir_t dir = lv_indev_get_gesture_dir(indev);
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
    ui_action_animations_destroy();
}

void create_screens(void) {
    create_screen_main();
}