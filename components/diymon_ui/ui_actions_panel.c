/*
 * Fichero: ./components/diymon_ui/ui_actions_panel.c
 * Fecha: 10/08/2025 - 04:45
 * Último cambio: Añadido log para clics y arrastres sobre el panel de acciones.
 * Descripción: El callback de eventos del panel ahora informa de más interacciones (clics, arrastres) para diferenciar claramente cuándo el usuario interactúa con la barra superior en lugar del fondo.
 */
#include "ui_actions_panel.h"
#include "diymon_ui_helpers.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "UI_PANEL";

static lv_obj_t *s_panel_obj;
static lv_obj_t *s_eat_btn, *s_gym_btn, *s_atk_btn;
static lv_timer_t *s_panel_hide_timer = NULL;

static void animate_panel_in(void);
static void animate_panel_out(void);
static void timer_auto_hide_callback(lv_timer_t *timer);
static void panel_anim_out_finished_cb(lv_anim_t *a);
static void panel_event_cb(lv_event_t *e);

void ui_actions_panel_create(lv_obj_t *parent) {
    char path_buffer[128];
    ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "");

    s_panel_obj = lv_obj_create(parent);
    lv_obj_set_scrollbar_mode(s_panel_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(s_panel_obj, 170, 64);
    lv_obj_set_style_bg_opa(s_panel_obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(s_panel_obj, 0, 0);
    lv_obj_set_style_radius(s_panel_obj, 0, 0);
    lv_obj_set_pos(s_panel_obj, 0, -lv_obj_get_height(s_panel_obj));
    lv_obj_set_flex_flow(s_panel_obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(s_panel_obj, 7, 0);
    lv_obj_set_style_pad_column(s_panel_obj, 7, 0);
    lv_obj_add_flag(s_panel_obj, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_add_event_cb(s_panel_obj, panel_event_cb, LV_EVENT_ALL, NULL);
    
    s_eat_btn = lv_btn_create(s_panel_obj);
    lv_obj_set_size(s_eat_btn, 50, 50);
    lv_obj_t *img_eat = lv_img_create(s_eat_btn);
    snprintf(path_buffer, sizeof(path_buffer), "S:/sdcard/buttons/ICON_EAT.bin");
    lv_img_set_src(img_eat, path_buffer);
    lv_obj_center(img_eat);

    s_gym_btn = lv_btn_create(s_panel_obj);
    lv_obj_set_size(s_gym_btn, 50, 50);
    lv_obj_t *img_gym = lv_img_create(s_gym_btn);
    snprintf(path_buffer, sizeof(path_buffer), "S:/sdcard/buttons/ICON_GYM.bin");
    lv_img_set_src(img_gym, path_buffer);
    lv_obj_center(img_gym);

    s_atk_btn = lv_btn_create(s_panel_obj);
    lv_obj_set_size(s_atk_btn, 50, 50);
    lv_obj_t *img_atk = lv_img_create(s_atk_btn);
    snprintf(path_buffer, sizeof(path_buffer), "S:/sdcard/buttons/ICON_ATK.bin");
    lv_img_set_src(img_atk, path_buffer);
    lv_obj_center(img_atk);

    lv_obj_move_foreground(s_panel_obj);
    ESP_LOGI(TAG, "Panel de acciones creado.");
}

lv_obj_t* ui_actions_panel_get_eat_btn(void) { return s_eat_btn; }
lv_obj_t* ui_actions_panel_get_gym_btn(void) { return s_gym_btn; }
lv_obj_t* ui_actions_panel_get_atk_btn(void) { return s_atk_btn; }

static void timer_auto_hide_callback(lv_timer_t *timer) {
    animate_panel_out();
    s_panel_hide_timer = NULL;
}

static void animate_panel_in(void) {
    if (s_panel_obj && lv_obj_has_flag(s_panel_obj, LV_OBJ_FLAG_HIDDEN)) {
        if (s_panel_hide_timer) lv_timer_del(s_panel_hide_timer);
        
        lv_obj_clear_flag(s_panel_obj, LV_OBJ_FLAG_HIDDEN);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, s_panel_obj);
        lv_anim_set_values(&a, -lv_obj_get_height(s_panel_obj), 0);
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_start(&a);

        s_panel_hide_timer = lv_timer_create(timer_auto_hide_callback, 5000, NULL);
        lv_timer_set_repeat_count(s_panel_hide_timer, 1);
    }
}

static void panel_anim_out_finished_cb(lv_anim_t *a) {
    lv_obj_add_flag(s_panel_obj, LV_OBJ_FLAG_HIDDEN);
}

static void animate_panel_out(void) {
    if (s_panel_obj && !lv_obj_has_flag(s_panel_obj, LV_OBJ_FLAG_HIDDEN)) {
        if (s_panel_hide_timer) {
            lv_timer_del(s_panel_hide_timer);
            s_panel_hide_timer = NULL;
        }
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, s_panel_obj);
        lv_anim_set_values(&a, 0, -lv_obj_get_height(s_panel_obj));
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
        lv_anim_set_ready_cb(&a, panel_anim_out_finished_cb);
        lv_anim_start(&a);
    }
}

void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_y) {
    if (dir == LV_DIR_BOTTOM && start_y >= 0 && start_y < 40) {
        ESP_LOGI(TAG, "Gesto (Fondo) -> Mostrar panel");
        animate_panel_in();
    }
}

static void panel_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();
    lv_point_t p;

    switch (code) {
        case LV_EVENT_PRESSED:
            lv_indev_get_point(indev, &p);
            ESP_LOGW(TAG, "EVENTO (Panel Acciones): Pulsado en -> X: %d, Y: %d", p.x, p.y);
            break;
        case LV_EVENT_CLICKED:
            lv_indev_get_point(indev, &p);
            ESP_LOGW(TAG, "EVENTO (Panel Acciones): Click en -> X: %d, Y: %d", p.x, p.y);
            break;
        case LV_EVENT_PRESSING:
            lv_indev_get_point(indev, &p);
            ESP_LOGI(TAG, "EVENTO (Panel Acciones): Arrastrando en -> X: %d, Y: %d", p.x, p.y);
            break;
        case LV_EVENT_GESTURE: {
            lv_dir_t dir = lv_indev_get_gesture_dir(indev);
            if (dir == LV_DIR_TOP) {
                ESP_LOGW(TAG, "GESTO (Panel Acciones): Arrastre ARRIBA -> Ocultar panel");
                animate_panel_out();
            }
            break;
        }
        default:
            break;
    }
}