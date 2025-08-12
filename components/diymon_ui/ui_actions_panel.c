// Fichero: ./components/diymon_ui/ui_actions_panel.c
// Fecha: 12/08/2025 - 14:00
// Último cambio: Renombrada la implementación de la función `get` del botón de configuración.
// Descripción: Se actualiza el nombre de la función `ui_actions_panel_get_enable_ftp_btn`
//              para que coincida con su nueva declaración en la cabecera, completando el refactor.

#include "ui_actions_panel.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "ui_idle_animation.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "UI_PANELS";

// --- Constantes de configuración ---
#define PANEL_AUTO_HIDE_DELAY 10000 
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
#define ANIM_TIME_MS 300
#define NUM_TOP_BUTTONS 3
#define NUM_SIDE_BUTTONS 5
#define EDGE_SWIPE_THRESHOLD 50

typedef enum {
    PANEL_STATE_HIDDEN,
    PANEL_STATE_PLAYER_VISIBLE,
    PANEL_STATE_ADMIN_VISIBLE,
    PANEL_STATE_SIDE_VISIBLE,
    PANEL_STATE_CONFIG_VISIBLE,
} panel_state_t;

static lv_obj_t *s_player_btns[NUM_TOP_BUTTONS];
static lv_obj_t *s_admin_btns[NUM_TOP_BUTTONS];
static lv_obj_t *s_side_btns[NUM_SIDE_BUTTONS];
static lv_obj_t *s_config_btns[NUM_TOP_BUTTONS];
static lv_timer_t *s_hide_timer = NULL;
static panel_state_t s_panel_state = PANEL_STATE_HIDDEN;
static bool s_is_animating = false;

// --- Declaraciones de funciones internas ---
static void animate_panel_in_top(lv_obj_t **buttons);
static void animate_panel_out_top(lv_obj_t **buttons);
static void animate_panel_in_side(lv_obj_t **buttons);
static void animate_panel_out_side(lv_obj_t **buttons);
static void timer_auto_hide_callback(lv_timer_t *timer);
static lv_obj_t* create_top_action_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index);
static lv_obj_t* create_side_action_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index);
static void button_event_cb(lv_event_t *e);
static void anim_ready_hide_cb(lv_anim_t *a);
static void animation_finish_cb(lv_anim_t *a);
static void last_button_out_anim_ready_cb(lv_anim_t *a);


static lv_obj_t* create_top_action_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_t *img = lv_img_create(btn);
    lv_img_set_src(img, ui_assets_get_icon(asset_id));
    lv_obj_center(img);
    lv_obj_set_pos(btn, (BUTTON_SIZE + BUTTON_PADDING) * index, -BUTTON_SIZE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_PRESSED, NULL);
    return btn;
}

static lv_obj_t* create_side_action_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_t *img = lv_img_create(btn);
    lv_img_set_src(img, ui_assets_get_icon(asset_id));
    lv_obj_center(img);
    lv_obj_set_pos(btn, -BUTTON_SIZE, BUTTON_PADDING + (BUTTON_SIZE + BUTTON_PADDING) * index);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_PRESSED, NULL);
    return btn;
}

void ui_actions_panel_create(lv_obj_t *parent) {
    s_player_btns[0] = create_top_action_button(parent, ASSET_ICON_EAT, 0);
    s_player_btns[1] = create_top_action_button(parent, ASSET_ICON_GYM, 1);
    s_player_btns[2] = create_top_action_button(parent, ASSET_ICON_ATK, 2);

    s_admin_btns[0] = create_top_action_button(parent, ASSET_ICON_BRIGHTNESS, 0);
    s_admin_btns[1] = create_top_action_button(parent, ASSET_ICON_SCREEN_OFF, 1);
    s_admin_btns[2] = create_top_action_button(parent, ASSET_ICON_ADMIN_PLACEHOLDER, 2);

    s_config_btns[0] = create_top_action_button(parent, ASSET_ICON_RESET_ALL, 0);
    s_config_btns[1] = create_top_action_button(parent, ASSET_ICON_ENABLE_FTP, 1); // El asset sigue siendo el mismo
    s_config_btns[2] = create_top_action_button(parent, ASSET_ICON_CONFIG_PLACEHOLDER, 2);

    s_side_btns[0] = create_side_action_button(parent, ASSET_ICON_EVO_FIRE, 0);
    s_side_btns[1] = create_side_action_button(parent, ASSET_ICON_EVO_WATER, 1);
    s_side_btns[2] = create_side_action_button(parent, ASSET_ICON_EVO_EARTH, 2);
    s_side_btns[3] = create_side_action_button(parent, ASSET_ICON_EVO_WIND, 3);
    s_side_btns[4] = create_side_action_button(parent, ASSET_ICON_EVO_BACK, 4);

    ESP_LOGI(TAG, "Todos los paneles de acción creados.");
}

lv_obj_t* ui_actions_panel_get_eat_btn(void) { return s_player_btns[0]; }
lv_obj_t* ui_actions_panel_get_gym_btn(void) { return s_player_btns[1]; }
lv_obj_t* ui_actions_panel_get_atk_btn(void) { return s_player_btns[2]; }
lv_obj_t* ui_actions_panel_get_brightness_btn(void) { return s_admin_btns[0]; }
lv_obj_t* ui_actions_panel_get_toggle_screen_btn(void) { return s_admin_btns[1]; }
lv_obj_t* ui_actions_panel_get_admin_placeholder_btn(void) { return s_admin_btns[2]; }
lv_obj_t* ui_actions_panel_get_reset_all_btn(void) { return s_config_btns[0]; }
lv_obj_t* ui_actions_panel_get_enable_config_mode_btn(void) { return s_config_btns[1]; } // Renombrado
lv_obj_t* ui_actions_panel_get_config_placeholder_btn(void) { return s_config_btns[2]; }
lv_obj_t* ui_actions_panel_get_evo_fire_btn(void) { return s_side_btns[0]; }
lv_obj_t* ui_actions_panel_get_evo_water_btn(void) { return s_side_btns[1]; }
lv_obj_t* ui_actions_panel_get_evo_earth_btn(void) { return s_side_btns[2]; }
lv_obj_t* ui_actions_panel_get_evo_wind_btn(void) { return s_side_btns[3]; }
lv_obj_t* ui_actions_panel_get_evo_back_btn(void) { return s_side_btns[4]; }


static void timer_auto_hide_callback(lv_timer_t *timer) {
    if (s_panel_state == PANEL_STATE_PLAYER_VISIBLE) animate_panel_out_top(s_player_btns);
    if (s_panel_state == PANEL_STATE_ADMIN_VISIBLE) animate_panel_out_top(s_admin_btns);
    if (s_panel_state == PANEL_STATE_CONFIG_VISIBLE) animate_panel_out_top(s_config_btns);
    if (s_panel_state == PANEL_STATE_SIDE_VISIBLE) animate_panel_out_side(s_side_btns);
    
    ESP_LOGD(TAG, "Auto-hide timer triggered. Resuming idle animation.");
    ui_idle_animation_resume();

    s_panel_state = PANEL_STATE_HIDDEN;
    s_hide_timer = NULL;
}

static void anim_ready_hide_cb(lv_anim_t *a) {
    lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN);
}

static void animation_finish_cb(lv_anim_t *a) {
    s_is_animating = false;
    ESP_LOGD(TAG, "Panel IN animation finished. Gesture lock released.");
}

static void last_button_out_anim_ready_cb(lv_anim_t *a) {
    lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN);
    s_is_animating = false;
    ESP_LOGD(TAG, "Panel OUT animation finished. Gesture lock released.");
}


static void animate_panel_in_top(lv_obj_t **buttons) {
    if (s_hide_timer) lv_timer_del(s_hide_timer);
    
    s_is_animating = true;
    for (int i = 0; i < NUM_TOP_BUTTONS; i++) {
        if (buttons[i]) {
            lv_obj_clear_flag(buttons[i], LV_OBJ_FLAG_HIDDEN);
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, buttons[i]);
            lv_anim_set_values(&a, lv_obj_get_y(buttons[i]), BUTTON_PADDING);
            lv_anim_set_time(&a, ANIM_TIME_MS);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
            lv_anim_set_delay(&a, i * 50);
            if (i == NUM_TOP_BUTTONS - 1) {
                lv_anim_set_ready_cb(&a, animation_finish_cb);
            }
            lv_anim_start(&a);
        }
    }
    s_hide_timer = lv_timer_create(timer_auto_hide_callback, PANEL_AUTO_HIDE_DELAY, NULL);
    lv_timer_set_repeat_count(s_hide_timer, 1);
}

static void animate_panel_out_top(lv_obj_t **buttons) {
    if (s_hide_timer) {
        lv_timer_del(s_hide_timer);
        s_hide_timer = NULL;
    }
    s_is_animating = true;
    for (int i = 0; i < NUM_TOP_BUTTONS; i++) {
        if (buttons[i]) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, buttons[i]);
            lv_anim_set_values(&a, lv_obj_get_y(buttons[i]), -BUTTON_SIZE);
            lv_anim_set_time(&a, ANIM_TIME_MS);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
            lv_anim_set_delay(&a, i * 50);
            if (i == NUM_TOP_BUTTONS - 1) {
                lv_anim_set_ready_cb(&a, last_button_out_anim_ready_cb);
            } else {
                lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
            }
            lv_anim_start(&a);
        }
    }
}

static void animate_panel_in_side(lv_obj_t **buttons) {
    if (s_hide_timer) lv_timer_del(s_hide_timer);
    
    s_is_animating = true;
    for (int i = 0; i < NUM_SIDE_BUTTONS; i++) {
        if (buttons[i]) {
            lv_obj_clear_flag(buttons[i], LV_OBJ_FLAG_HIDDEN);
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, buttons[i]);
            lv_anim_set_values(&a, lv_obj_get_x(buttons[i]), BUTTON_PADDING);
            lv_anim_set_time(&a, ANIM_TIME_MS);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
            lv_anim_set_delay(&a, i * 50);
            if (i == NUM_SIDE_BUTTONS - 1) {
                lv_anim_set_ready_cb(&a, animation_finish_cb);
            }
            lv_anim_start(&a);
        }
    }
    s_hide_timer = lv_timer_create(timer_auto_hide_callback, PANEL_AUTO_HIDE_DELAY, NULL);
    lv_timer_set_repeat_count(s_hide_timer, 1);
}

static void animate_panel_out_side(lv_obj_t **buttons) {
    if (s_hide_timer) {
        lv_timer_del(s_hide_timer);
        s_hide_timer = NULL;
    }
    s_is_animating = true;
    for (int i = 0; i < NUM_SIDE_BUTTONS; i++) {
        if (buttons[i]) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, buttons[i]);
            lv_anim_set_values(&a, lv_obj_get_x(buttons[i]), -BUTTON_SIZE);
            lv_anim_set_time(&a, ANIM_TIME_MS);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
            lv_anim_set_delay(&a, i * 50);
            if (i == NUM_SIDE_BUTTONS - 1) {
                lv_anim_set_ready_cb(&a, last_button_out_anim_ready_cb);
            } else {
                lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
            }
            lv_anim_start(&a);
        }
    }
}

void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_x, lv_coord_t start_y) {
    if (s_is_animating) {
        ESP_LOGD(TAG, "Animation in progress, gesture ignored.");
        return;
    }

    ESP_LOGD(TAG, "Gesture: Dir=%d, X=%d, Y=%d, State=%d", dir, start_x, start_y, s_panel_state);

    switch(s_panel_state) {
        case PANEL_STATE_HIDDEN:
            if ((dir == LV_DIR_BOTTOM && start_y < EDGE_SWIPE_THRESHOLD) || (dir == LV_DIR_RIGHT && start_x < EDGE_SWIPE_THRESHOLD)) {
                // El gesto es válido, no hacemos nada más aquí.
            } else {
                ESP_LOGD(TAG, "Invalid gesture. Resuming idle animation.");
                ui_idle_animation_resume();
                return;
            }

            if (dir == LV_DIR_BOTTOM) {
                animate_panel_in_top(s_player_btns);
                s_panel_state = PANEL_STATE_PLAYER_VISIBLE;
            } else if (dir == LV_DIR_RIGHT) {
                animate_panel_in_side(s_side_btns);
                s_panel_state = PANEL_STATE_SIDE_VISIBLE;
            }
            break;

        case PANEL_STATE_PLAYER_VISIBLE:
            if (dir == LV_DIR_BOTTOM) {
                animate_panel_out_top(s_player_btns);
                animate_panel_in_top(s_admin_btns);
                s_panel_state = PANEL_STATE_ADMIN_VISIBLE;
            } else if (dir == LV_DIR_TOP) {
                animate_panel_out_top(s_player_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;

        case PANEL_STATE_ADMIN_VISIBLE:
            if (dir == LV_DIR_BOTTOM) {
                animate_panel_out_top(s_admin_btns);
                animate_panel_in_top(s_config_btns);
                s_panel_state = PANEL_STATE_CONFIG_VISIBLE;
            } else if (dir == LV_DIR_TOP) {
                animate_panel_out_top(s_admin_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
            
        case PANEL_STATE_CONFIG_VISIBLE:
            if (dir == LV_DIR_TOP) {
                animate_panel_out_top(s_config_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
        
        case PANEL_STATE_SIDE_VISIBLE:
            if (dir == LV_DIR_LEFT) {
                animate_panel_out_side(s_side_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
    }
}

static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        if (s_hide_timer) {
            lv_timer_reset(s_hide_timer);
        }
    }
}