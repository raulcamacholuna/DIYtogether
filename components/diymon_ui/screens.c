/*
 * Fichero: ./components/diymon_ui/screens.c
 * Fecha: 13/08/2025 - 09:10
 * Último cambio: Añadida la captura de la coordenada X del toque.
 * Descripción: Se modifica el manejador de eventos de la pantalla para capturar no solo la coordenada 'y' del inicio del toque, sino también la 'x'. Esto es necesario para que el gestor de paneles pueda detectar gestos de borde (edge swipes).
 */
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "esp_log.h"
#include "screen_state.h" 
#include "bsp_api.h"      

static const char *TAG = "SCREENS";

// --- Variables para la detección manual de doble toque ---
static uint8_t g_click_count = 0;
static lv_timer_t *g_double_click_timer = NULL;

lv_obj_t *g_idle_animation_obj = NULL;
lv_obj_t *g_main_screen_obj = NULL;

static lv_coord_t touch_start_x = -1;
static lv_coord_t touch_start_y = -1;

static void main_screen_event_cb(lv_event_t *e);

/**
 * @brief Callback del temporizador que se activa si no hay un segundo toque.
 */
static void double_click_timer_cb(lv_timer_t *timer) {
    g_click_count = 0;
    g_double_click_timer = NULL;
}

void create_screen_main(void) {
    g_main_screen_obj = lv_obj_create(NULL);
    lv_obj_clear_flag(g_main_screen_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(g_main_screen_obj, 170, 320);
    lv_obj_add_flag(g_main_screen_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(g_main_screen_obj, main_screen_event_cb, LV_EVENT_ALL, NULL);

    ui_action_animations_create(g_main_screen_obj);
    g_idle_animation_obj = ui_idle_animation_start(g_main_screen_obj);
    ui_actions_panel_create(g_main_screen_obj);

    ESP_LOGI(TAG, "Pantalla principal creada delegando en módulos.");
}

static void main_screen_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();

    switch(code) {
        case LV_EVENT_PRESSED: {
            lv_point_t p;
            lv_indev_get_point(indev, &p);
            touch_start_x = p.x;
            touch_start_y = p.y;
            break;
        }
        case LV_EVENT_RELEASED:
            touch_start_x = -1;
            touch_start_y = -1;
            break;
        case LV_EVENT_GESTURE: {
            if (!g_screen_is_off) { 
                lv_dir_t dir = lv_indev_get_gesture_dir(indev);
                ui_actions_panel_handle_gesture(dir, touch_start_x, touch_start_y);
            }
            break;
        }
        case LV_EVENT_CLICKED: { // Se usa CLICKED en lugar de DOUBLE_CLICKED
            if (g_screen_is_off) {
                g_click_count++;
                if (g_click_count == 1) {
                    // Primer clic, iniciar temporizador de 300ms
                    g_double_click_timer = lv_timer_create(double_click_timer_cb, 300, NULL);
                    lv_timer_set_repeat_count(g_double_click_timer, 1);
                } else if (g_click_count == 2) {
                    // Segundo clic dentro de la ventana del temporizador
                    if (g_double_click_timer) {
                        lv_timer_del(g_double_click_timer);
                        g_double_click_timer = NULL;
                    }
                    ESP_LOGI(TAG, "Doble toque (manual) detectado. ¡Encendiendo pantalla!");
                    bsp_display_turn_on();
                    g_screen_is_off = false;
                    g_click_count = 0; // Reiniciar contador
                }
            }
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