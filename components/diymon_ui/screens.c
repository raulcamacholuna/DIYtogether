/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\screens.c
# Fecha: $timestamp
# Último cambio: Reactivada la detección de gestos habilitando el scroll.
# Descripción: Se ha corregido el problema que impedía la detección de gestos. La línea `lv_obj_clear_flag(..., LV_OBJ_FLAG_SCROLLABLE)` deshabilitaba el mecanismo de scroll de LVGL, que es necesario para generar eventos de deslizamiento. Se ha reemplazado por una configuración que permite el scroll para la detección de gestos pero oculta la barra de desplazamiento visual.
*/
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "ui_telemetry.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include "bsp_api.h"
#include "screen_manager.h"
#include "ui_asset_loader.h"

static const char *TAG = "SCREENS";

static uint8_t g_click_count = 0;
static lv_timer_t *g_double_click_timer = NULL;
static lv_timer_t *s_resume_idle_timer = NULL; // Temporizador para reanudar el idle

lv_obj_t *g_idle_animation_obj = NULL;
lv_obj_t *g_main_screen_obj = NULL;

static lv_coord_t touch_start_x = -1;
static lv_coord_t touch_start_y = -1;

static void main_screen_event_cb(lv_event_t *e);

static void double_click_timer_cb(lv_timer_t *timer) {
    g_click_count = 0;
    g_double_click_timer = NULL;
}

static void resume_idle_timer_cb(lv_timer_t *timer) {
    ESP_LOGD(TAG, "Temporizador de reanudación de idle disparado.");
    ui_idle_animation_resume();
    s_resume_idle_timer = NULL; // El temporizador se auto-elimina (repeat_count=1)
}

void create_screen_main(void) {
    g_main_screen_obj = lv_obj_create(NULL);
    lv_obj_set_size(g_main_screen_obj, 170, 320);
    lv_obj_add_flag(g_main_screen_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(g_main_screen_obj, main_screen_event_cb, LV_EVENT_ALL, NULL);
    
    // [CORRECCIÓN] Habilitar la detección de gestos permitiendo el scroll, pero sin mostrar la barra.
    // La línea `lv_obj_clear_flag(g_main_screen_obj, LV_OBJ_FLAG_SCROLLABLE);` impedía los gestos.
    lv_obj_set_scroll_dir(g_main_screen_obj, LV_DIR_ALL);
    lv_obj_set_scrollbar_mode(g_main_screen_obj, LV_SCROLLBAR_MODE_OFF);

    ui_helpers_load_background(g_main_screen_obj);

    ui_action_animations_create(g_main_screen_obj);
    g_idle_animation_obj = ui_idle_animation_start(g_main_screen_obj);
    ui_actions_panel_create(g_main_screen_obj);
    ui_telemetry_create(g_main_screen_obj);

    ESP_LOGI(TAG, "Pantalla principal creada delegando en módulos.");
}

static void main_screen_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();

    switch(code) {
        case LV_EVENT_PRESSED: {
            if (s_resume_idle_timer) {
                lv_timer_del(s_resume_idle_timer);
                s_resume_idle_timer = NULL;
            }

            lv_point_t p;
            lv_indev_get_point(indev, &p);
            touch_start_x = p.x;
            touch_start_y = p.y;
            if (!screen_manager_is_off()) {
                ESP_LOGD(TAG, "Touch detected, pausing idle animation.");
                ui_idle_animation_pause();
            }
            break;
        }
        case LV_EVENT_RELEASED: {
            touch_start_x = -1;
            touch_start_y = -1;
            
            if (!screen_manager_is_off() && s_resume_idle_timer == NULL) {
                 ESP_LOGD(TAG, "Touch released. Starting 500ms timer to resume idle animation.");
                s_resume_idle_timer = lv_timer_create(resume_idle_timer_cb, 500, NULL);
                lv_timer_set_repeat_count(s_resume_idle_timer, 1);
            }
            break;
        }
        case LV_EVENT_GESTURE: {
            if (!screen_manager_is_off()) { 
                lv_dir_t dir = lv_indev_get_gesture_dir(indev);
                ui_actions_panel_handle_gesture(dir, touch_start_x, touch_start_y);
            }
            break;
        }
        case LV_EVENT_CLICKED: {
            if (screen_manager_is_off()) {
                g_click_count++;
                if (g_click_count == 1) {
                    g_double_click_timer = lv_timer_create(double_click_timer_cb, 300, NULL);
                    lv_timer_set_repeat_count(g_double_click_timer, 1);
                } else if (g_click_count == 2) {
                    if (g_double_click_timer) {
                        lv_timer_del(g_double_click_timer);
                        g_double_click_timer = NULL;
                    }
                    ESP_LOGI(TAG, "Doble toque detectado. Encendiendo pantalla...");
                    screen_manager_turn_on();
                    g_click_count = 0;
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
    ui_telemetry_destroy();
    ui_assets_deinit();
}

void create_screens(void) {
    create_screen_main();
}
