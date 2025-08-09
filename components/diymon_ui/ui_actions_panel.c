/*
  Fichero: ./components/diymon_ui/ui_actions_panel.c
  Fecha: 10/08/2025 - 04:15
  Último cambio: Corregido el callback de finalización de animación.
  Descripción: Se ha añadido una función de callback personalizada ('anim_ready_hide_cb')
               para ocultar los botones correctamente al finalizar su animación de
               salida. Esto soluciona el error de compilación 'lv_obj_add_flag_cb undeclared'
               y restaura la funcionalidad de ocultar los iconos.
*/
#include "ui_actions_panel.h"
#include "ui_asset_loader.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "UI_PANEL";

// --- Constantes de configuración ---
#define PANEL_AUTO_HIDE_DELAY 10000 // 10 segundos
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10 // Espacio entre botones
#define ANIM_TIME_MS 300

// --- Variables estáticas del módulo ---
static lv_obj_t *s_action_btns[ASSET_COUNT];
static lv_timer_t *s_hide_timer = NULL;
static bool s_is_panel_visible = false;

// --- Declaraciones de funciones internas ---
static void animate_buttons_in(void);
static void animate_buttons_out(void);
static void timer_auto_hide_callback(lv_timer_t *timer);
static lv_obj_t* create_action_button(lv_obj_t *parent, ui_asset_id_t asset_id);
static void button_event_cb(lv_event_t *e);
static void anim_ready_hide_cb(lv_anim_t *a);


/**
 * @brief Crea un botón de acción como un objeto independiente.
 */
static lv_obj_t* create_action_button(lv_obj_t *parent, ui_asset_id_t asset_id) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, BUTTON_SIZE, BUTTON_SIZE);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);

    lv_obj_t *img = lv_img_create(btn);
    const lv_img_dsc_t* icon_dsc = ui_assets_get_icon(asset_id);
    lv_img_set_src(img, icon_dsc);
    lv_obj_center(img);
    
    lv_obj_set_pos(btn, (BUTTON_SIZE + BUTTON_PADDING) * asset_id, -BUTTON_SIZE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_PRESSED, NULL);

    return btn;
}

void ui_actions_panel_create(lv_obj_t *parent) {
    s_action_btns[ASSET_ICON_EAT] = create_action_button(parent, ASSET_ICON_EAT);
    s_action_btns[ASSET_ICON_GYM] = create_action_button(parent, ASSET_ICON_GYM);
    s_action_btns[ASSET_ICON_ATK] = create_action_button(parent, ASSET_ICON_ATK);

    ESP_LOGI(TAG, "Botones de acción creados como objetos independientes.");
}

lv_obj_t* ui_actions_panel_get_eat_btn(void) { return s_action_btns[ASSET_ICON_EAT]; }
lv_obj_t* ui_actions_panel_get_gym_btn(void) { return s_action_btns[ASSET_ICON_GYM]; }
lv_obj_t* ui_actions_panel_get_atk_btn(void) { return s_action_btns[ASSET_ICON_ATK]; }

static void timer_auto_hide_callback(lv_timer_t *timer) {
    animate_buttons_out();
    s_hide_timer = NULL; 
}

static void animate_buttons_in(void) {
    if (s_is_panel_visible) return;
    s_is_panel_visible = true;
    
    if (s_hide_timer) lv_timer_del(s_hide_timer);
    
    for (int i = 0; i < ASSET_COUNT; i++) {
        if (s_action_btns[i]) {
            lv_obj_clear_flag(s_action_btns[i], LV_OBJ_FLAG_HIDDEN);
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, s_action_btns[i]);
            lv_anim_set_values(&a, lv_obj_get_y(s_action_btns[i]), BUTTON_PADDING);
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

/**
 * @brief Callback para ocultar el objeto al final de la animación.
 */
static void anim_ready_hide_cb(lv_anim_t *a) {
    lv_obj_add_flag((lv_obj_t *)a->var, LV_OBJ_FLAG_HIDDEN);
}

static void animate_buttons_out(void) {
    if (!s_is_panel_visible) return;
    s_is_panel_visible = false;
    
    if (s_hide_timer) {
        lv_timer_del(s_hide_timer);
        s_hide_timer = NULL;
    }

    for (int i = 0; i < ASSET_COUNT; i++) {
        if (s_action_btns[i]) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, s_action_btns[i]);
            lv_anim_set_values(&a, lv_obj_get_y(s_action_btns[i]), -BUTTON_SIZE);
            lv_anim_set_time(&a, ANIM_TIME_MS);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
            lv_anim_set_delay(&a, i * 50);
            lv_anim_set_ready_cb(&a, anim_ready_hide_cb);
            lv_anim_start(&a);
        }
    }
}

void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_y) {
    if (s_is_panel_visible) {
        if (dir == LV_DIR_TOP) {
             ESP_LOGI(TAG, "Gesto (Arriba) -> Ocultar botones");
             animate_buttons_out();
        }
    } else {
        // Ahora, cualquier gesto hacia abajo activa el panel, sin importar dónde empiece.
        if (dir == LV_DIR_BOTTOM) {
            ESP_LOGI(TAG, "Gesto (Abajo) -> Mostrar botones");
            animate_buttons_in();
        }
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