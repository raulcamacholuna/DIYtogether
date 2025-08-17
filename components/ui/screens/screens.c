/* Fecha: 17/08/2025 - 02:33  */
/* Fichero: components/ui/screens/screens.c */
/* Último cambio: Actualizadas las inclusiones y llamadas a funciones para reflejar la refactorización del módulo de telemetría a ui/core/. */
/* Descripción: Gestor de la pantalla principal. Se ha actualizado para incluir 'telemetry_manager.h' y llamar a 'telemetry_manager_create/destroy' en lugar de las antiguas funciones 'ui_telemetry_*', adaptándose a la nueva estructura del núcleo de la UI. */
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "telemetry_manager.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include "bsp_api.h"
#include "screen_manager.h"
#include "ui_asset_loader.h"

static const char *TAG = "SCREENS";

static lv_timer_t *s_resume_idle_timer = NULL;

lv_obj_t *g_idle_animation_obj = NULL;
lv_obj_t *g_main_screen_obj = NULL;

static lv_coord_t touch_start_x = -1;
static lv_coord_t touch_start_y = -1;

static void main_screen_event_cb(lv_event_t *e);

static void resume_idle_timer_cb(lv_timer_t *timer) {
    ESP_LOGD(TAG, "Temporizador de reanudación de idle disparado.");
    ui_idle_animation_resume();
    s_resume_idle_timer = NULL;
}

void create_screen_main(void) {
    g_main_screen_obj = lv_obj_create(NULL);
    lv_obj_set_size(g_main_screen_obj, 170, 320);
    lv_obj_add_flag(g_main_screen_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(g_main_screen_obj, main_screen_event_cb, LV_EVENT_ALL, NULL);
    
    lv_obj_set_scroll_dir(g_main_screen_obj, LV_DIR_ALL);
    lv_obj_set_scrollbar_mode(g_main_screen_obj, LV_SCROLLBAR_MODE_OFF);

    ui_helpers_load_background(g_main_screen_obj);

    ui_action_animations_create(g_main_screen_obj);
    g_idle_animation_obj = ui_idle_animation_start(g_main_screen_obj);
    ui_actions_panel_create(g_main_screen_obj);
    telemetry_manager_create(g_main_screen_obj);

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
                ui_idle_animation_pause();
            }
            break;
        }
        case LV_EVENT_RELEASED: {
            touch_start_x = -1;
            touch_start_y = -1;
            
            if (!screen_manager_is_off() && s_resume_idle_timer == NULL) {
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
            // Si la pantalla no está apagada, un clic en cualquier
            // parte que no sea un botón, oculta los paneles de acción.
            if (!screen_manager_is_off()) {
                ESP_LOGD(TAG, "Click en la pantalla, ocultando paneles.");
                ui_actions_panel_hide_all();
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
    telemetry_manager_destroy();
    ui_assets_deinit();
}

void create_screens(void) {
    create_screen_main();
}
