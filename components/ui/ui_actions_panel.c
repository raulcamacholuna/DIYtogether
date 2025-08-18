/* Fecha: 18/08/2025 - 10:15  */
/* Fichero: components/ui/ui_actions_panel.c */
/* Último cambio: Añadido el flag LV_OBJ_FLAG_GESTURE_BUBBLE a todos los paneles para permitir la propagación de eventos de gesto. */
/* Descripción: Se ha solucionado un error por el cual los gestos de swipe no funcionaban si se iniciaban sobre un panel ya visible. Al añadir el flag `LV_OBJ_FLAG_GESTURE_BUBBLE`, los paneles ahora permiten que los eventos de gesto "burbujeen" hasta la pantalla principal, que es la que contiene la lógica de transición, asegurando un comportamiento consistente. */

#include "ui_actions_panel.h"
#include "ui_idle_animation.h"
#include "esp_log.h"
#include <stdio.h>

// --- Inclusión directa de los módulos de botones ---
#include "buttons/btn_1.h"
#include "buttons/btn_2.h"
#include "buttons/btn_3.h"
#include "buttons/btn_4.h"
#include "buttons/btn_5.h"
#include "buttons/btn_6.h"
#include "buttons/btn_7.h"
#include "buttons/btn_8.h"
#include "buttons/btn_9.h"
#include "buttons/evo_1.h"
#include "buttons/evo_2.h"
#include "buttons/evo_3.h"
#include "buttons/evo_4.h"
#include "buttons/evo_5.h"

static const char *TAG = "UI_PANELS";

// --- Constantes de configuración ---
#define PANEL_AUTO_HIDE_DELAY 10000 
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
#define ANIM_TIME_MS 300
#define EDGE_SWIPE_THRESHOLD 50
#define TOP_PANEL_WIDTH (BUTTON_SIZE * 3 + BUTTON_PADDING * 2)
#define TOP_PANEL_HEIGHT (BUTTON_SIZE)
#define SIDE_PANEL_WIDTH (BUTTON_SIZE)
#define SIDE_PANEL_HEIGHT (BUTTON_SIZE * 5 + BUTTON_PADDING * 4)


typedef enum {
    PANEL_STATE_HIDDEN,
    PANEL_STATE_PLAYER_VISIBLE,
    PANEL_STATE_ADMIN_VISIBLE,
    PANEL_STATE_CONFIG_VISIBLE,
    PANEL_STATE_SIDE_VISIBLE,
} panel_state_t;

// --- Variables estáticas del módulo ---
static lv_obj_t *s_panel_player;
static lv_obj_t *s_panel_admin;
static lv_obj_t *s_panel_config;
static lv_obj_t *s_panel_evo;

static lv_timer_t *s_hide_timer = NULL;
static panel_state_t s_panel_state = PANEL_STATE_HIDDEN;
static bool s_is_animating = false;

// --- Declaraciones de funciones internas ---
static void animate_panel_in_top(lv_obj_t *panel);
static void animate_panel_out_top(lv_obj_t *panel);
static void animate_panel_in_side(lv_obj_t *panel);
static void animate_panel_out_side(lv_obj_t *panel);
static void timer_auto_hide_callback(lv_timer_t *timer);
static void animation_finish_cb(lv_anim_t *a);

// --- Implementación de Funciones Públicas ---

void ui_actions_panel_create(lv_obj_t *parent) {
    // --- Crear Contenedor Panel Player ---
    s_panel_player = lv_obj_create(parent);
    lv_obj_remove_style_all(s_panel_player);
    lv_obj_set_size(s_panel_player, TOP_PANEL_WIDTH, TOP_PANEL_HEIGHT);
    lv_obj_align(s_panel_player, LV_ALIGN_TOP_MID, 0, -TOP_PANEL_HEIGHT);
    lv_obj_add_flag(s_panel_player, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(s_panel_player, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(s_panel_player, LV_OBJ_FLAG_GESTURE_BUBBLE); // [CORRECCIÓN CLAVE]
    btn_1_create(s_panel_player);
    btn_2_create(s_panel_player);
    btn_3_create(s_panel_player);
    ESP_LOGI(TAG, "Panel 'Player' creado.");

    // --- Crear Contenedor Panel Admin ---
    s_panel_admin = lv_obj_create(parent);
    lv_obj_remove_style_all(s_panel_admin);
    lv_obj_set_size(s_panel_admin, TOP_PANEL_WIDTH, TOP_PANEL_HEIGHT);
    lv_obj_align(s_panel_admin, LV_ALIGN_TOP_MID, 0, -TOP_PANEL_HEIGHT);
    lv_obj_add_flag(s_panel_admin, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(s_panel_admin, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(s_panel_admin, LV_OBJ_FLAG_GESTURE_BUBBLE); // [CORRECCIÓN CLAVE]
    btn_4_create(s_panel_admin);
    btn_5_create(s_panel_admin);
    btn_6_create(s_panel_admin);
    ESP_LOGI(TAG, "Panel 'Admin' creado.");

    // --- Crear Contenedor Panel Config ---
    s_panel_config = lv_obj_create(parent);
    lv_obj_remove_style_all(s_panel_config);
    lv_obj_set_size(s_panel_config, TOP_PANEL_WIDTH, TOP_PANEL_HEIGHT);
    lv_obj_align(s_panel_config, LV_ALIGN_TOP_MID, 0, -TOP_PANEL_HEIGHT);
    lv_obj_add_flag(s_panel_config, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(s_panel_config, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(s_panel_config, LV_OBJ_FLAG_GESTURE_BUBBLE); // [CORRECCIÓN CLAVE]
    btn_7_create(s_panel_config);
    btn_8_create(s_panel_config);
    btn_9_create(s_panel_config);
    ESP_LOGI(TAG, "Panel 'Config' creado.");

    // --- Crear Contenedor Panel Evolución ---
    s_panel_evo = lv_obj_create(parent);
    lv_obj_remove_style_all(s_panel_evo);
    lv_obj_set_size(s_panel_evo, SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT);
    lv_obj_align(s_panel_evo, LV_ALIGN_LEFT_MID, -SIDE_PANEL_WIDTH, 0);
    lv_obj_add_flag(s_panel_evo, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(s_panel_evo, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(s_panel_evo, LV_OBJ_FLAG_GESTURE_BUBBLE); // [CORRECCIÓN CLAVE]
    evo_1_create(s_panel_evo);
    evo_2_create(s_panel_evo);
    evo_3_create(s_panel_evo);
    evo_4_create(s_panel_evo);
    evo_5_create(s_panel_evo);
    ESP_LOGI(TAG, "Panel 'Evo' creado.");
}

static void animation_finish_cb(lv_anim_t *a) { 
    s_is_animating = false; 
    if(lv_obj_get_y((lv_obj_t *)a->var) < 0 || lv_obj_get_x((lv_obj_t *)a->var) < 0) {
        lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN);
    }
}

static void animate_panel_in_top(lv_obj_t *panel) {
    if (s_hide_timer) lv_timer_del(s_hide_timer);
    s_is_animating = true;
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, panel);
    lv_anim_set_values(&a, lv_obj_get_y(panel), BUTTON_PADDING);
    lv_anim_set_time(&a, ANIM_TIME_MS);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&a, animation_finish_cb);
    lv_anim_start(&a);

    s_hide_timer = lv_timer_create(timer_auto_hide_callback, PANEL_AUTO_HIDE_DELAY, NULL);
    lv_timer_set_repeat_count(s_hide_timer, 1);
}

static void animate_panel_out_top(lv_obj_t *panel) {
    if (s_hide_timer) { lv_timer_del(s_hide_timer); s_hide_timer = NULL; }
    s_is_animating = true;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, panel);
    lv_anim_set_values(&a, lv_obj_get_y(panel), -TOP_PANEL_HEIGHT);
    lv_anim_set_time(&a, ANIM_TIME_MS);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&a, animation_finish_cb);
    lv_anim_start(&a);
}

static void animate_panel_in_side(lv_obj_t *panel) {
    if (s_hide_timer) lv_timer_del(s_hide_timer);
    s_is_animating = true;
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, panel);
    lv_anim_set_values(&a, lv_obj_get_x(panel), BUTTON_PADDING);
    lv_anim_set_time(&a, ANIM_TIME_MS);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&a, animation_finish_cb);
    lv_anim_start(&a);

    s_hide_timer = lv_timer_create(timer_auto_hide_callback, PANEL_AUTO_HIDE_DELAY, NULL);
    lv_timer_set_repeat_count(s_hide_timer, 1);
}

static void animate_panel_out_side(lv_obj_t *panel) {
    if (s_hide_timer) { lv_timer_del(s_hide_timer); s_hide_timer = NULL; }
    s_is_animating = true;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, panel);
    lv_anim_set_values(&a, lv_obj_get_x(panel), -SIDE_PANEL_WIDTH);
    lv_anim_set_time(&a, ANIM_TIME_MS);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&a, animation_finish_cb);
    lv_anim_start(&a);
}

static void timer_auto_hide_callback(lv_timer_t *timer) {
    ui_actions_panel_hide_all();
    s_hide_timer = NULL;
}

void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_x, lv_coord_t start_y) {
    if (s_is_animating) return;
    if (s_hide_timer) lv_timer_reset(s_hide_timer);

    switch(s_panel_state) {
        case PANEL_STATE_HIDDEN:
            if (dir == LV_DIR_BOTTOM && start_y < EDGE_SWIPE_THRESHOLD) {
                animate_panel_in_top(s_panel_player);
                s_panel_state = PANEL_STATE_PLAYER_VISIBLE;
            } else if (dir == LV_DIR_RIGHT && start_x < EDGE_SWIPE_THRESHOLD) {
                animate_panel_in_side(s_panel_evo);
                s_panel_state = PANEL_STATE_SIDE_VISIBLE;
            } else {
                ui_idle_animation_resume();
            }
            break;
        case PANEL_STATE_PLAYER_VISIBLE:
            if (dir == LV_DIR_BOTTOM) {
                animate_panel_out_top(s_panel_player);
                animate_panel_in_top(s_panel_admin);
                s_panel_state = PANEL_STATE_ADMIN_VISIBLE;
            } else if (dir == LV_DIR_TOP) {
                animate_panel_out_top(s_panel_player);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
        case PANEL_STATE_ADMIN_VISIBLE:
            if (dir == LV_DIR_BOTTOM) {
                animate_panel_out_top(s_panel_admin);
                animate_panel_in_top(s_panel_config);
                s_panel_state = PANEL_STATE_CONFIG_VISIBLE;
            } else if (dir == LV_DIR_TOP) {
                animate_panel_out_top(s_panel_admin);
                animate_panel_in_top(s_panel_player);
                s_panel_state = PANEL_STATE_PLAYER_VISIBLE;
            }
            break;
        case PANEL_STATE_CONFIG_VISIBLE:
             if (dir == LV_DIR_TOP) {
                animate_panel_out_top(s_panel_config);
                animate_panel_in_top(s_panel_admin);
                s_panel_state = PANEL_STATE_ADMIN_VISIBLE;
            }
            break;
        case PANEL_STATE_SIDE_VISIBLE:
            if (dir == LV_DIR_LEFT) {
                animate_panel_out_side(s_panel_evo);
                s_panel_state = PANEL_STATE_HIDDEN;
                ui_idle_animation_resume();
            }
            break;
    }
}

void ui_actions_panel_hide_all(void) {
    if (s_panel_state == PANEL_STATE_HIDDEN || s_is_animating) return;

    if (s_panel_state == PANEL_STATE_PLAYER_VISIBLE) animate_panel_out_top(s_panel_player);
    else if (s_panel_state == PANEL_STATE_ADMIN_VISIBLE) animate_panel_out_top(s_panel_admin);
    else if (s_panel_state == PANEL_STATE_CONFIG_VISIBLE) animate_panel_out_top(s_panel_config);
    else if (s_panel_state == PANEL_STATE_SIDE_VISIBLE) animate_panel_out_side(s_panel_evo);
    
    s_panel_state = PANEL_STATE_HIDDEN;
    if (s_hide_timer) { lv_timer_del(s_hide_timer); s_hide_timer = NULL; }
    ui_idle_animation_resume();
}
