/*
  Fichero: ./components/diymon_ui_panels/action_panels.c
  Fecha: 12/08/2025 - 07:45
  Último cambio: Creación del componente.
  Descripción: Corazón de la nueva arquitectura de paneles. Centraliza la definición 
               de los botones, sus iconos y acciones en un único lugar fácil de editar.
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
#define NUM_TOP_BUTTONS 3
#define NUM_SIDE_BUTTONS 5
#define EDGE_SWIPE_THRESHOLD 50

// --- Definición de estados del panel ---
typedef enum {
    PANEL_STATE_HIDDEN,
    PANEL_STATE_PLAYER_VISIBLE,
    PANEL_STATE_ADMIN_VISIBLE,
    PANEL_STATE_CONFIG_VISIBLE,
    PANEL_STATE_SIDE_VISIBLE,
} panel_state_t;

// --- Variables estáticas del módulo ---
static lv_obj_t *s_player_btns[NUM_TOP_BUTTONS];
static lv_obj_t *s_admin_btns[NUM_TOP_BUTTONS];
static lv_obj_t *s_config_btns[NUM_TOP_BUTTONS];
static lv_obj_t *s_side_btns[NUM_SIDE_BUTTONS];

static lv_timer_t *s_hide_timer = NULL;
static panel_state_t s_panel_state = PANEL_STATE_HIDDEN;

// --- Declaraciones de funciones internas ---
static void animate_panel_in_top(lv_obj_t **buttons);
static void animate_panel_out_top(lv_obj_t **buttons);
static void animate_panel_in_side(lv_obj_t **buttons);
static void animate_panel_out_side(lv_obj_t **buttons);
static void timer_auto_hide_callback(lv_timer_t *timer);
static lv_obj_t* create_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index, bool is_top);
static void button_event_cb(lv_event_t *e);
static void anim_ready_hide_cb(lv_anim_t *a);

/**
 * @brief Función de utilidad para crear un botón de acción.
 */
static lv_obj_t* create_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index, bool is_top) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);

    lv_obj_t *img = lv_img_create(btn);
    lv_img_set_src(img, ui_assets_get_icon(asset_id));
    lv_obj_center(img);

    if (is_top) {
        lv_obj_set_pos(btn, (BUTTON_SIZE + BUTTON_PADDING) * index, -BUTTON_SIZE);
    } else {
        lv_obj_set_pos(btn, -BUTTON_SIZE, BUTTON_PADDING + (BUTTON_SIZE + BUTTON_PADDING) * index);
    }
    
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
    return btn;
}

/**
 * @brief Orquestador principal de la creación de todos los paneles y botones.
 */
void ui_actions_panel_create(lv_obj_t *parent) {
    // Panel 1: Jugador
    s_player_btns[0] = create_button(parent, ASSET_BTN_1, 0, true);
    s_player_btns[1] = create_button(parent, ASSET_BTN_2, 1, true);
    s_player_btns[2] = create_button(parent, ASSET_BTN_3, 2, true);
    lv_obj_add_event_cb(s_player_btns[0], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_COMER);
    lv_obj_add_event_cb(s_player_btns[1], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EJERCICIO);
    lv_obj_add_event_cb(s_player_btns[2], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ATACAR);

    // Panel 2: Admin
    s_admin_btns[0] = create_button(parent, ASSET_BTN_4, 0, true);
    s_admin_btns[1] = create_button(parent, ASSET_BTN_5, 1, true);
    s_admin_btns[2] = create_button(parent, ASSET_BTN_6, 2, true);
    lv_obj_add_event_cb(s_admin_btns[0], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_BRIGHTNESS_CYCLE);
    lv_obj_add_event_cb(s_admin_btns[1], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_TOGGLE_SCREEN);
    lv_obj_add_event_cb(s_admin_btns[2], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ADMIN_PLACEHOLDER);

    // Panel 3: Configuración
    s_config_btns[0] = create_button(parent, ASSET_BTN_7, 0, true);
    s_config_btns[1] = create_button(parent, ASSET_BTN_8, 1, true);
    s_config_btns[2] = create_button(parent, ASSET_BTN_9, 2, true);
    lv_obj_add_event_cb(s_config_btns[0], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_RESET_ALL);
    lv_obj_add_event_cb(s_config_btns[1], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_ENABLE_FTP);
    lv_obj_add_event_cb(s_config_btns[2], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_CONFIG_PLACEHOLDER);

    // Panel 4: Lateral (Evolución)
    s_side_btns[0] = create_button(parent, ASSET_EVO_1, 0, false);
    s_side_btns[1] = create_button(parent, ASSET_EVO_2, 1, false);
    s_side_btns[2] = create_button(parent, ASSET_EVO_3, 2, false);
    s_side_btns[3] = create_button(parent, ASSET_EVO_4, 3, false);
    s_side_btns[4] = create_button(parent, ASSET_EVO_5, 4, false);
    lv_obj_add_event_cb(s_side_btns[0], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_FIRE);
    lv_obj_add_event_cb(s_side_btns[1], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_WATER);
    lv_obj_add_event_cb(s_side_btns[2], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_EARTH);
    lv_obj_add_event_cb(s_side_btns[3], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_WIND);
    lv_obj_add_event_cb(s_side_btns[4], button_event_cb, LV_EVENT_CLICKED, (void*)ACTION_ID_EVO_BACK);
    
    ESP_LOGI(TAG, "Paneles de acción modulares creados.");
}

/**
 * @brief Callback que se ejecuta cuando la animación de ocultar termina.
 */
static void anim_ready_hide_cb(lv_anim_t *a) {
    lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN);
}

/**
 * @brief Lógica para animar la entrada de un panel superior.
 */
static void animate_panel_in_top(lv_obj_t **buttons) {
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
            lv_anim_start(&a);
        }
    }
}

/**
 * @brief Lógica para animar la salida de un panel superior.
 */
static void animate_panel_out_top(lv_obj_t **buttons) {
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
            lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
            lv_anim_start(&a);
        }
    }
}

/**
 * @brief Lógica para animar la entrada de un panel lateral.
 */
static void animate_panel_in_side(lv_obj_t **buttons) {
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
            lv_anim_start(&a);
        }
    }
}

/**
 * @brief Lógica para animar la salida de un panel lateral.
 */
static void animate_panel_out_side(lv_obj_t **buttons) {
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
            lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
            lv_anim_start(&a);
        }
    }
}

/**
 * @brief Callback del temporizador de auto-ocultado.
 */
static void timer_auto_hide_callback(lv_timer_t *timer) {
    ESP_LOGI(TAG, "Temporizador de auto-ocultado disparado.");
    if (s_panel_state == PANEL_STATE_PLAYER_VISIBLE) animate_panel_out_top(s_player_btns);
    if (s_panel_state == PANEL_STATE_ADMIN_VISIBLE) animate_panel_out_top(s_admin_btns);
    if (s_panel_state == PANEL_STATE_CONFIG_VISIBLE) animate_panel_out_top(s_config_btns);
    if (s_panel_state == PANEL_STATE_SIDE_VISIBLE) animate_panel_out_side(s_side_btns);
    s_panel_state = PANEL_STATE_HIDDEN;
    s_hide_timer = NULL;
}

/**
 * @brief Máquina de estados que gestiona la visibilidad de los paneles según los gestos.
 */
void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_x, lv_coord_t start_y) {
    if (s_hide_timer) {
        lv_timer_reset(s_hide_timer);
    } else {
        s_hide_timer = lv_timer_create(timer_auto_hide_callback, PANEL_AUTO_HIDE_DELAY, NULL);
        lv_timer_set_repeat_count(s_hide_timer, 1);
    }

    switch(s_panel_state) {
        case PANEL_STATE_HIDDEN:
            if (dir == LV_DIR_BOTTOM && start_y < EDGE_SWIPE_THRESHOLD) {
                animate_panel_in_top(s_player_btns);
                s_panel_state = PANEL_STATE_PLAYER_VISIBLE;
            } else if (dir == LV_DIR_RIGHT && start_x < EDGE_SWIPE_THRESHOLD) {
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
            }
            break;
            
        case PANEL_STATE_CONFIG_VISIBLE:
            if (dir == LV_DIR_TOP) {
                animate_panel_out_top(s_config_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
            }
            break;
        
        case PANEL_STATE_SIDE_VISIBLE:
            if (dir == LV_DIR_LEFT) {
                animate_panel_out_side(s_side_btns);
                s_panel_state = PANEL_STATE_HIDDEN;
            }
            break;
    }
}

/**
 * @brief Callback para todos los eventos de clic en los botones de acción.
 */
static void button_event_cb(lv_event_t *e) {
    if (s_hide_timer) {
        lv_timer_reset(s_hide_timer);
    }
    diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
    execute_diymon_action(action_id);
}