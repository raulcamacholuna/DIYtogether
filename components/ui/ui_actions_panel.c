/* Fecha: 17/08/2025 - 01:31 
# Fichero: components/ui/ui_actions_panel.c
# Último cambio: Refactorizado para usar el nuevo 'buttons_manager' y eliminar la lógica de creación de botones local.
# Descripción: Este fichero ahora solo gestiona la LÓGICA de los paneles (animaciones, estado, gestos). La creación y gestión de los botones individuales se ha delegado al 'buttons_manager'. Las funciones de animación ahora obtienen los manejadores de los botones del manager en lugar de usar arrays estáticos locales, completando la refactorización.
*/
#include "ui_actions_panel.h"
#include "buttons_manager.h" // <-- NUEVO include
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
    PANEL_STATE_CONFIG_VISIBLE,
    PANEL_STATE_SIDE_VISIBLE,
} panel_state_t;

// --- Variables estáticas del módulo ---
static lv_timer_t *s_hide_timer = NULL;
static panel_state_t s_panel_state = PANEL_STATE_HIDDEN;
static bool s_is_animating = false;

// --- Declaraciones de funciones internas ---
static void animate_panel_in_top(lv_obj_t **buttons);
static void animate_panel_out_top(lv_obj_t **buttons);
static void animate_panel_in_side(lv_obj_t **buttons);
static void animate_panel_out_side(lv_obj_t **buttons);
static void timer_auto_hide_callback(lv_timer_t *timer);
static void anim_ready_hide_cb(lv_anim_t *a);
static void animation_finish_cb(lv_anim_t *a);
static void last_button_out_anim_ready_cb(lv_anim_t *a);

// --- Implementación de Funciones Públicas ---

void ui_actions_panel_create(lv_obj_t *parent) {
    // La creación de botones se delega completamente al manager.
    ui_buttons_init(parent);
    ESP_LOGI(TAG, "Creación de botones delegada al buttons_manager.");
}

// --- Lógica de Animación (sin cambios, ya que aceptan arrays) ---
static void anim_ready_hide_cb(lv_anim_t *a) { lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN); }
static void animation_finish_cb(lv_anim_t *a) { s_is_animating = false; }
static void last_button_out_anim_ready_cb(lv_anim_t *a) { lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN); s_is_animating = false; }

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
            if (i == NUM_TOP_BUTTONS - 1) lv_anim_set_ready_cb(&a, animation_finish_cb);
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
            if (i == NUM_TOP_BUTTONS - 1) lv_anim_set_ready_cb(&a, last_button_out_anim_ready_cb);
            else lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
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
            if (i == NUM_SIDE_BUTTONS - 1) lv_anim_set_ready_cb(&a, animation_finish_cb);
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
            if (i == NUM_SIDE_BUTTONS - 1) lv_anim_set_ready_cb(&a, last_button_out_anim_ready_cb);
            else lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
            lv_anim_start(&a);
        }
    }
}


// --- Lógica de Gestos y Temporizadores (Modificada para usar el manager) ---

static void timer_auto_hide_callback(lv_timer_t *timer) {
    ui_actions_panel_hide_all(); // Usar la función pública para centralizar
    s_hide_timer = NULL;
}

void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_x, lv_coord_t start_y) {
    if (s_is_animating) return;

    if (s_hide_timer) {
        lv_timer_reset(s_hide_timer);
    } else {
        s_hide_timer = lv_timer_create(timer_auto_hide_callback, PANEL_AUTO_HIDE_DELAY, NULL);
        lv_timer_set_repeat_count(s_hide_timer, 1);
    }

    // Definición de los grupos de botones obtenidos del manager
    lv_obj_t* player_buttons[] = { ui_buttons_get_btn_1(), ui_buttons_get_btn_2(), ui_buttons_get_btn_3() };
    lv_obj_t* admin_buttons[] = { ui_buttons_get_btn_4(), ui_buttons_get_btn_5(), ui_buttons_get_btn_6() };
    lv_obj_t* config_buttons[] = { ui_buttons_get_btn_7(), ui_buttons_get_btn_8(), ui_buttons_get_btn_9() };
    lv_obj_t* side_buttons[] = { ui_buttons_get_evo_1(), ui_buttons_get_evo_2(), ui_buttons_get_evo_3(), ui_buttons_get_evo_4(), ui_buttons_get_evo_5() };

    switch(s_panel_state) {
        case PANEL_STATE_HIDDEN:
            if (dir == LV_DIR_BOTTOM && start_y < EDGE_SWIPE_THRESHOLD) {
                animate_panel_in_top(player_buttons);
                s_panel_state = PANEL_STATE_PLAYER_VISIBLE;
            } else if (dir == LV_DIR_RIGHT && start_x < EDGE_SWIPE_THRESHOLD) {
                animate_panel_in_side(side_buttons);
                s_panel_state = PANEL_STATE_SIDE_VISIBLE;
            } else {
                ui_idle_animation_resume();
            }
            break;
        case PANEL_STATE_PLAYER_VISIBLE:
            if (dir == LV_DIR_BOTTOM) {
                animate_panel_out_top(player_buttons);
                animate_panel_in_top(admin_buttons);
                s_panel_state = PANEL_STATE_ADMIN_VISIBLE;
            } else if (dir == LV_DIR_TOP) {
                animate_panel_out_top(player_buttons);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
        case PANEL_STATE_ADMIN_VISIBLE:
            if (dir == LV_DIR_BOTTOM) {
                animate_panel_out_top(admin_buttons);
                animate_panel_in_top(config_buttons);
                s_panel_state = PANEL_STATE_CONFIG_VISIBLE;
            } else if (dir == LV_DIR_TOP) {
                animate_panel_out_top(admin_buttons);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
        case PANEL_STATE_CONFIG_VISIBLE:
             if (dir == LV_DIR_TOP) {
                animate_panel_out_top(config_buttons);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
        case PANEL_STATE_SIDE_VISIBLE:
            if (dir == LV_DIR_LEFT) {
                animate_panel_out_side(side_buttons);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
    }
}

void ui_actions_panel_hide_all(void) {
    if (s_panel_state == PANEL_STATE_HIDDEN || s_is_animating) {
        return;
    }
    ESP_LOGD(TAG, "Forzando el ocultado de todos los paneles visibles.");

    // Definición de los grupos de botones para ocultar
    lv_obj_t* player_buttons[] = { ui_buttons_get_btn_1(), ui_buttons_get_btn_2(), ui_buttons_get_btn_3() };
    lv_obj_t* admin_buttons[] = { ui_buttons_get_btn_4(), ui_buttons_get_btn_5(), ui_buttons_get_btn_6() };
    lv_obj_t* config_buttons[] = { ui_buttons_get_btn_7(), ui_buttons_get_btn_8(), ui_buttons_get_btn_9() };
    lv_obj_t* side_buttons[] = { ui_buttons_get_evo_1(), ui_buttons_get_evo_2(), ui_buttons_get_evo_3(), ui_buttons_get_evo_4(), ui_buttons_get_evo_5() };

    if (s_panel_state == PANEL_STATE_PLAYER_VISIBLE) animate_panel_out_top(player_buttons);
    else if (s_panel_state == PANEL_STATE_ADMIN_VISIBLE) animate_panel_out_top(admin_buttons);
    else if (s_panel_state == PANEL_STATE_CONFIG_VISIBLE) animate_panel_out_top(config_buttons);
    else if (s_panel_state == PANEL_STATE_SIDE_VISIBLE) animate_panel_out_side(side_buttons);
    
    s_panel_state = PANEL_STATE_HIDDEN;
    if (s_hide_timer) {
        lv_timer_del(s_hide_timer);
        s_hide_timer = NULL;
    }
    ui_idle_animation_resume();
}
