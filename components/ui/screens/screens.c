/* Fichero: components/ui/screens/screens.c */
/* Descripción: Diagnóstico de Causa Raíz: La pantalla de 1.47" tiene una resolución física ligeramente mayor que su resolución lógica de 172px, lo que provoca que se muestren píxeles no inicializados (ruido visual) en los bordes laterales. Solución Definitiva: Se ha modificado la creación de la pantalla principal ('g_main_screen_obj') para que tenga un fondo negro sólido y opaco, sin bordes ni padding. Al establecer explícitamente el color de fondo de todo el objeto de la pantalla, se asegura que LVGL limpie todo el framebuffer a negro en cada ciclo de renderizado, eliminando eficazmente el ruido visual y creando bordes negros limpios en los laterales. */
/* Último cambio: 21/08/2025 - 19:09 */
#include "screens.h"
#include "ui_idle_animation.h"
#include "ui_actions_panel.h"
#include "ui_action_animations.h"
#include "telemetry/telemetry_manager.h"
#include "helpers.h"
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

// --- Declaraciones de funciones internas ---
static void main_screen_event_cb(lv_event_t *e);
static void cleanup_main_screen_resources(void);

static void resume_idle_timer_cb(lv_timer_t *timer) {
    ui_idle_animation_resume();
    s_resume_idle_timer = NULL;
}

static void cleanup_main_screen_resources(void) {
    ESP_LOGI(TAG, "[CLEANUP] Entrando en cleanup_main_screen_resources...");
    
    ESP_LOGI(TAG, "[CLEANUP] -> Deteniendo animación de idle...");
    ui_idle_animation_stop();
    ESP_LOGI(TAG, "[CLEANUP] <- Animación de idle detenida.");

    ESP_LOGI(TAG, "[CLEANUP] -> Destruyendo gestor de telemetría...");
    telemetry_manager_destroy();
    ESP_LOGI(TAG, "[CLEANUP] <- Gestor de telemetría destruido.");

    ESP_LOGI(TAG, "[CLEANUP] -> Liberando buffer de animación compartido...");
    ui_action_animations_destroy();
    ESP_LOGI(TAG, "[CLEANUP] <- Buffer de animación liberado.");

    ESP_LOGI(TAG, "[CLEANUP] -> Desinicializando assets...");
    ui_assets_deinit();
    ESP_LOGI(TAG, "[CLEANUP] <- Assets desinicializados.");

    ESP_LOGI(TAG, "[CLEANUP] Saliendo de cleanup_main_screen_resources.");
}

void create_screen_main(void) {
    g_main_screen_obj = lv_obj_create(NULL);
    lv_obj_remove_style_all(g_main_screen_obj);
    lv_obj_set_style_bg_color(g_main_screen_obj, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_main_screen_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(g_main_screen_obj, 0, 0);
    lv_obj_set_style_border_width(g_main_screen_obj, 0, 0);
    
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

    ESP_LOGI(TAG, "Pantalla principal creada.");
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
            if (!screen_manager_is_off()) {
                ui_actions_panel_hide_all();
            }
            break;
        }
        default:
            break;
    }
}

void delete_screen_main(void) {
    ESP_LOGI(TAG, "Entrando en delete_screen_main...");
    if (g_main_screen_obj) {
        ESP_LOGI(TAG, "-> Llamando a cleanup_main_screen_resources...");
        cleanup_main_screen_resources();
        ESP_LOGI(TAG, "<- Retorno de cleanup_main_screen_resources.");
        
        ESP_LOGI(TAG, "-> Llamando a lv_obj_del(g_main_screen_obj)...");
        lv_obj_del(g_main_screen_obj);
        ESP_LOGI(TAG, "<- Retorno de lv_obj_del.");

        g_main_screen_obj = NULL;
        g_idle_animation_obj = NULL;
    } else {
        ESP_LOGW(TAG, "delete_screen_main llamado pero g_main_screen_obj es NULL.");
    }
    ESP_LOGI(TAG, "Saliendo de delete_screen_main.");
}

void create_screens(void) {
    create_screen_main();
}
