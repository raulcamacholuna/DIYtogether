/*
  Fichero: ./components/diymon_ui/ui_actions_panel.c
  Fecha: 12/08/2025 - 11:36
  Último cambio: Implementada la máquina de estados para gestionar ambos paneles.
  Descripción: Se ha refactorizado por completo para manejar un panel de jugador y un panel de administración. Incluye una máquina de estados para controlar la visibilidad (oculto, jugador visible, admin visible) y funciones de animación parametrizadas.
*/
#include "ui_actions_panel.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "UI_PANELS";

// --- Constantes de configuración ---
#define PANEL_AUTO_HIDE_DELAY 10000 
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
#define ANIM_TIME_MS 300
#define NUM_BUTTONS_PER_PANEL 3

// --- Máquina de estados para los paneles ---
typedef enum {
    PANEL_STATE_HIDDEN,
    PANEL_STATE_PLAYER_VISIBLE,
    PANEL_STATE_ADMIN_VISIBLE,
} panel_state_t;

// --- Variables estáticas del módulo ---
static lv_obj_t *s_player_btns[NUM_BUTTONS_PER_PANEL];
static lv_obj_t *s_admin_btns[NUM_BUTTONS_PER_PANEL];
static lv_timer_t *s_hide_timer = NULL;
static panel_state_t s_panel_state = PANEL_STATE_HIDDEN;

// --- Declaraciones de funciones internas ---
static void animate_panel_in(lv_obj_t **buttons);
static void animate_panel_out(lv_obj_t **buttons);
static void timer_auto_hide_callback(lv_timer_t *timer);
static lv_obj_t* create_action_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index);
static void button_event_cb(lv_event_t *e);
static void anim_ready_hide_cb(lv_anim_t *a);

static lv_obj_t* create_action_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);

    lv_obj_t *img = lv_img_create(btn);
    const lv_img_dsc_t* icon_dsc = ui_assets_get_icon(asset_id);
    lv_img_set_src(img, icon_dsc);
    lv_obj_center(img);
    
    lv_obj_set_pos(btn, (BUTTON_SIZE + BUTTON_PADDING) * index, -BUTTON_SIZE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_PRESSED, NULL);
    return btn;
}

void ui_actions_panel_create(lv_obj_t *parent) {
    // Crear botones del panel de jugador
    s_player_btns[0] = create_action_button(parent, ASSET_ICON_EAT, 0);
    s_player_btns[1] = create_action_button(parent, ASSET_ICON_GYM, 1);
    s_player_btns[2] = create_action_button(parent, ASSET_ICON_ATK, 2);

    // Crear botones del panel de admin
    s_admin_btns[0] = create_action_button(parent, ASSET_ICON_LVL_DOWN, 0);
    s_admin_btns[1] = create_action_button(parent, ASSET_ICON_SCREEN_OFF, 1);
    s_admin_btns[2] = create_action_button(parent, ASSET_ICON_LVL_UP, 2);

    ESP_LOGI(TAG, "Paneles de jugador y admin creados.");
}

// --- Getters para botones ---
lv_obj_t* ui_actions_panel_get_eat_btn(void) { return s_player_btns[0]; }
lv_obj_t* ui_actions_panel_get_gym_btn(void) { return s_player_btns[1]; }
lv_obj_t* ui_actions_panel_get_atk_btn(void) { return s_player_btns[2]; }
lv_obj_t* ui_actions_panel_get_lvl_down_btn(void) { return s_admin_btns[0]; }
lv_obj_t* ui_actions_panel_get_screen_off_btn(void) { return s_admin_btns[1]; }
lv_obj_t* ui_actions_panel_get_lvl_up_btn(void) { return s_admin_btns[2]; }


static void timer_auto_hide_callback(lv_timer_t *timer) {
    ESP_LOGI(TAG, "Temporizador de ocultación automática activado.");
    if (s_panel_state == PANEL_STATE_PLAYER_VISIBLE) {
        animate_panel_out(s_player_btns);
    } else if (s_panel_state == PANEL_STATE_ADMIN_VISIBLE) {
        animate_panel_out(s_admin_btns);
    }
    s_panel_state = PANEL_STATE_HIDDEN;
    s_hide_timer = NULL;
}

// --- Funciones de animación parametrizadas ---
static void animate_panel_in(lv_obj_t **buttons) {
    if (s_hide_timer) lv_timer_del(s_hide_timer);
    
    for (int i = 0; i < NUM_BUTTONS_PER_PANEL; i++) {
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
            lv_anim_start(&a);
        }
    }
    s_hide_timer = lv_timer_create(timer_auto_hide_callback, PANEL_AUTO_HIDE_DELAY, NULL);
    lv_timer_set_repeat_count(s_hide_timer, 1);
}

static void anim_ready_hide_cb(lv_anim_t *a) {
    lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN);
}

static void animate_panel_out(lv_obj_t **buttons) {
    if (s_hide_timer) {
        lv_timer_del(s_hide_timer);
        s_hide_timer = NULL;
    }

    for (int i = 0; i < NUM_BUTTONS_PER_PANEL; i++) {
        if (buttons[i]) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, buttons[i]);
            lv_anim_set_values(&a, lv_obj_get_y(buttons[i]), -BUTTON_SIZE);
            lv_anim_set_time(&a, ANIM_TIME_MS);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
            lv_anim_set_delay(&a, i * 50);
            lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
            lv_anim_start(&a);
        }
    }
}

// --- Lógica principal de gestos ---
void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_y) {
    switch (s_panel_state) {
        case PANEL_STATE_HIDDEN:
            if (dir == LV_DIR_BOTTOM) {
                ESP_LOGI(TAG, "Gesto: Oculto -> Jugador");
                animate_panel_in(s_player_btns);
                s_panel_state = PANEL_STATE_PLAYER_VISIBLE;
            }
            break;

        case PANEL_STATE_PLAYER_VISIBLE:
            if (dir == LV_DIR_TOP) {
                ESP_LOGI(TAG, "Gesto: Jugador -> Oculto");
                animate_panel_out(s_player_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
            } else if (dir == LV_DIR_BOTTOM) {
                ESP_LOGI(TAG, "Gesto: Jugador -> Admin");
                animate_panel_out(s_player_btns);
                animate_panel_in(s_admin_btns);
                s_panel_state = PANEL_STATE_ADMIN_VISIBLE;
            }
            break;

        case PANEL_STATE_ADMIN_VISIBLE:
            if (dir == LV_DIR_TOP || dir == LV_DIR_BOTTOM) {
                ESP_LOGI(TAG, "Gesto: Admin -> Oculto");
                animate_panel_out(s_admin_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
            }
            break;
    }
}

static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        if (s_hide_timer) {
            lv_timer_reset(s_hide_timer);
            ESP_LOGI(TAG, "Botón tocado. Reiniciando temporizador de ocultación.");
        }
    }
}