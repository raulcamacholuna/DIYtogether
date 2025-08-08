/*
 * Fichero: ./components/diymon_ui/screens.c
 * Fecha: 08/08/2025 - 17:00
 * Último cambio: Reimplementada la lógica de gestos basándose en las coordenadas de inicio del toque.
 * Descripción: Se elimina el objeto de área de gestos y se implementa un manejador de eventos central en la pantalla principal. Este manejador registra la coordenada 'Y' inicial y activa el panel solo si un gesto descendente comienza en la zona superior de la pantalla, proporcionando una solución robusta y fiable.
 */

#include "ui_priv.h"
#include "diymon_ui_helpers.h" 
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

objects_t objects;
static const char *TAG = "SCREENS";

// --- DECLARACIONES DE FUNCIONES Y VARIABLES ESTÁTICAS ---
static lv_timer_t *panel_hide_timer = NULL;
static lv_coord_t touch_start_y = -1; // Variable para almacenar la coordenada Y inicial del toque.

static void animate_panel_in(void);
static void animate_panel_out(void);
static void main_screen_event_cb(lv_event_t *e);
static void timer_auto_hide_callback(lv_timer_t *timer);
static void panel_anim_out_finished_cb(lv_anim_t *a);

static bool check_asset_exists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f) { fclose(f); return true; } 
    else { ESP_LOGE(TAG, "FALLO: No se pudo abrir el fichero en '%s'", path); return false; }
}

void create_screen_main() {
    objects.main = lv_obj_create(NULL);
    lv_obj_clear_flag(objects.main, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(objects.main, 170, 320);
    // Habilitamos la capacidad de ser clickeado para que reciba todos los eventos de entrada.
    lv_obj_add_flag(objects.main, LV_OBJ_FLAG_CLICKABLE);
    // Se asigna el manejador de eventos principal a la pantalla.
    lv_obj_add_event_cb(objects.main, main_screen_event_cb, LV_EVENT_ALL, NULL);

    char path_buffer[128];
    // ... Carga de assets (sin cambios)...
    {
        objects.idle = lv_gif_create(objects.main);
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "diymon.gif");
        if (check_asset_exists(path_buffer)) { lv_gif_set_src(objects.idle, path_buffer); }
        lv_obj_align(objects.idle, LV_ALIGN_CENTER, 0, 0);
    }
    // ... más assets ...
    {
        objects.comiendo = lv_img_create(objects.main);
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "manzana.png");
        if (check_asset_exists(path_buffer)) { lv_img_set_src(objects.comiendo, path_buffer); }
        lv_obj_align(objects.comiendo, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(objects.comiendo, LV_OBJ_FLAG_HIDDEN);
    }
    {
        objects.ejercicio = lv_img_create(objects.main);
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "pesas.png");
        if (check_asset_exists(path_buffer)) { lv_img_set_src(objects.ejercicio, path_buffer); }
        lv_obj_align(objects.ejercicio, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(objects.ejercicio, LV_OBJ_FLAG_HIDDEN);
    }
    {
        objects.ataque = lv_img_create(objects.main);
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ataque.png");
        if (check_asset_exists(path_buffer)) { lv_img_set_src(objects.ataque, path_buffer); }
        lv_obj_align(objects.ataque, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(objects.ataque, LV_OBJ_FLAG_HIDDEN);
    }

    {
        objects.actions_panel = lv_obj_create(objects.main);
        lv_obj_set_scrollbar_mode(objects.actions_panel, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_size(objects.actions_panel, 170, 64);
        lv_obj_set_style_bg_opa(objects.actions_panel, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(objects.actions_panel, 0, 0);
        lv_obj_set_style_radius(objects.actions_panel, 0, 0);
        lv_obj_set_pos(objects.actions_panel, 0, -lv_obj_get_height(objects.actions_panel));
        lv_obj_set_flex_flow(objects.actions_panel, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_all(objects.actions_panel, 7, 0);
        lv_obj_set_style_pad_column(objects.actions_panel, 7, 0);
        lv_obj_add_flag(objects.actions_panel, LV_OBJ_FLAG_HIDDEN);
        
        {
            objects.comer = lv_btn_create(objects.actions_panel);
            lv_obj_set_size(objects.comer, 50, 50);
            lv_obj_t *img = lv_img_create(objects.comer);
            ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ICON_EAT.bin");
            if (check_asset_exists(path_buffer)) { lv_img_set_src(img, path_buffer); }
            lv_obj_center(img);
        }
        {
            objects.pesas = lv_btn_create(objects.actions_panel);
            lv_obj_set_size(objects.pesas, 50, 50);
            lv_obj_t *img = lv_img_create(objects.pesas);
            ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ICON_GYM.bin");
            if (check_asset_exists(path_buffer)) { lv_img_set_src(img, path_buffer); }
            lv_obj_center(img);
        }
        {
            objects.atacar = lv_btn_create(objects.actions_panel);
            lv_obj_set_size(objects.atacar, 50, 50);
            lv_obj_t *img = lv_img_create(objects.atacar);
            ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ICON_ATK.bin");
            if (check_asset_exists(path_buffer)) { lv_img_set_src(img, path_buffer); }
            lv_obj_center(img);
        }
    }
}

static void timer_auto_hide_callback(lv_timer_t *timer) {
    ESP_LOGI(TAG, "Temporizador de auto-ocultado activado. Ocultando panel.");
    animate_panel_out();
    panel_hide_timer = NULL;
}

static void animate_panel_in(void) {
    if (objects.actions_panel && lv_obj_has_flag(objects.actions_panel, LV_OBJ_FLAG_HIDDEN)) {
        if (panel_hide_timer) lv_timer_del(panel_hide_timer);
        
        lv_obj_clear_flag(objects.actions_panel, LV_OBJ_FLAG_HIDDEN);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, objects.actions_panel);
        lv_anim_set_values(&a, -lv_obj_get_height(objects.actions_panel), 0);
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_start(&a);

        panel_hide_timer = lv_timer_create(timer_auto_hide_callback, 5000, NULL);
        lv_timer_set_repeat_count(panel_hide_timer, 1);
    }
}

static void panel_anim_out_finished_cb(lv_anim_t *a) {
    lv_obj_add_flag(objects.actions_panel, LV_OBJ_FLAG_HIDDEN);
}

static void animate_panel_out(void) {
    if (objects.actions_panel && !lv_obj_has_flag(objects.actions_panel, LV_OBJ_FLAG_HIDDEN)) {
        if (panel_hide_timer) {
            lv_timer_del(panel_hide_timer);
            panel_hide_timer = NULL;
        }
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, objects.actions_panel);
        lv_anim_set_values(&a, 0, -lv_obj_get_height(objects.actions_panel));
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
        lv_anim_set_ready_cb(&a, panel_anim_out_finished_cb);
        lv_anim_start(&a);
    }
}

/**
 * @brief Manejador de eventos principal para la pantalla.
 * 
 * Orquesta la lógica de gestos basándose en las coordenadas del toque.
 */
static void main_screen_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();

    switch(code) {
        case LV_EVENT_PRESSED: {
            // Al presionar, se almacena la coordenada Y inicial.
            lv_point_t p;
            lv_indev_get_point(indev, &p);
            touch_start_y = p.y;
            break;
        }
        case LV_EVENT_GESTURE: {
            lv_dir_t dir = lv_indev_get_gesture_dir(indev);
            if (dir == LV_DIR_BOTTOM) {
                // Si el gesto es hacia abajo Y empezó en la zona superior (Y < 40), se muestra el panel.
                if (touch_start_y >= 0 && touch_start_y < 40) {
                    ESP_LOGW(TAG, "Logic -> GESTURE DOWN en zona superior (Y=%d). MOSTRANDO panel.", (int)touch_start_y);
                    animate_panel_in();
                }
            } else if (dir == LV_DIR_TOP) {
                // Si el gesto es hacia arriba, se oculta el panel (si está visible).
                ESP_LOGW(TAG, "Logic -> GESTURE UP. OCULTANDO panel.");
                animate_panel_out();
            }
            break;
        }
        case LV_EVENT_RELEASED: {
            // Al soltar, se resetea la coordenada de inicio.
            touch_start_y = -1;
            break;
        }
        default:
            break;
    }
}

void delete_screen_main() {
    if (objects.main) {
        lv_obj_del(objects.main);
    }
    if (panel_hide_timer) {
        lv_timer_del(panel_hide_timer);
        panel_hide_timer = NULL;
    }
    ui_helpers_free_background_buffer();
    memset(&objects, 0, sizeof(objects_t));
}

void create_screens() {
    create_screen_main();
}