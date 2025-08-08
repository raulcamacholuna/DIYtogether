/*
 * Fichero: ./components/diymon_ui/screens.c
 * Fecha: 08/08/2025 - 15:35
 * Último cambio: Deshabilitado el scroll en el panel de acciones superior.
 * Descripción: Creación de los elementos visuales de la UI. Se previene la aparición de barras de scroll en el panel de botones.
 */

#include "ui_priv.h"
#include "diymon_ui_helpers.h" 
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

objects_t objects;
static const char *TAG = "SCREENS";

// --- DECLARACIONES DE FUNCIONES ESTÁTICAS ---
static void animate_panel_in(void);
static void animate_panel_out(void);
static void screen_gesture_event_cb(lv_event_t *e);
static void panel_gesture_event_cb(lv_event_t *e);

/**
 * @brief Comprueba la existencia de un fichero en la ruta especificada.
 * @param path Ruta completa al fichero.
 * @return true si el fichero existe y se puede abrir, false en caso contrario.
 */
static bool check_asset_exists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f) {
        fclose(f);
        ESP_LOGI(TAG, "ÉXITO: Fichero encontrado en '%s'", path);
        return true;
    } else {
        ESP_LOGE(TAG, "FALLO: No se pudo abrir el fichero en '%s'", path);
        return false;
    }
}

void create_screen_main() {
    lv_obj_t *main_screen = lv_obj_create(NULL);
    objects.main = main_screen;
    lv_obj_clear_flag(main_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(main_screen, 170, 320);

    lv_obj_add_flag(main_screen, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(main_screen, screen_gesture_event_cb, LV_EVENT_GESTURE, NULL);

    lv_obj_t *parent_obj = main_screen;
    char path_buffer[128];

    ESP_LOGI(TAG, "--- Iniciando carga de assets de animación e imágenes ---");

    // Objeto para la animación de reposo (sigue siendo un GIF)
    {
        lv_obj_t *gif_obj = lv_gif_create(parent_obj);
        objects.idle = gif_obj;
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "diymon.gif");
        if (check_asset_exists(path_buffer)) { lv_gif_set_src(gif_obj, path_buffer); }
        lv_obj_align(gif_obj, LV_ALIGN_CENTER, 0, 0);
    }
    // Objeto para la acción de comer (ahora una imagen estática)
    {
        lv_obj_t *img_obj = lv_img_create(parent_obj);
        objects.comiendo = img_obj;
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "manzana.png");
        if (check_asset_exists(path_buffer)) { lv_img_set_src(img_obj, path_buffer); }
        lv_obj_align(img_obj, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(img_obj, LV_OBJ_FLAG_HIDDEN);
    }
    // Objeto para la acción de ejercicio (ahora una imagen estática)
    {
        lv_obj_t *img_obj = lv_img_create(parent_obj);
        objects.ejercicio = img_obj;
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "pesas.png");
        if (check_asset_exists(path_buffer)) { lv_img_set_src(img_obj, path_buffer); }
        lv_obj_align(img_obj, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(img_obj, LV_OBJ_FLAG_HIDDEN);
    }
    // Objeto para la acción de ataque (ahora una imagen estática)
    {
        lv_obj_t *img_obj = lv_img_create(parent_obj);
        objects.ataque = img_obj;
        ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ataque.png");
        if (check_asset_exists(path_buffer)) { lv_img_set_src(img_obj, path_buffer); }
        lv_obj_align(img_obj, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(img_obj, LV_OBJ_FLAG_HIDDEN);
    }

    ESP_LOGI(TAG, "--- Iniciando carga de iconos de botones ---");

    {
        lv_obj_t *panel = lv_obj_create(parent_obj);
        objects.actions_panel = panel;

        // Se elimina la capacidad de scroll del panel para evitar que aparezcan
        // las barras de desplazamiento si el contenido excede el tamaño del contenedor.
        lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_set_size(panel, 170, 64);
        lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(panel, 0, 0);
        lv_obj_set_style_radius(panel, 0, 0);
        lv_obj_set_pos(panel, 0, -lv_obj_get_height(panel));
        lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_top(panel, 7, 0);
        lv_obj_set_style_pad_left(panel, 7, 0);
        lv_obj_set_style_pad_column(panel, 7, 0);
        lv_obj_add_event_cb(panel, panel_gesture_event_cb, LV_EVENT_GESTURE, NULL);

        {
            lv_obj_t *btn = lv_btn_create(panel);
            objects.comer = btn;
            lv_obj_set_size(btn, 50, 50);
            lv_obj_t *img = lv_img_create(btn);
            ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ICON_EAT.bin");
            if (check_asset_exists(path_buffer)) { lv_img_set_src(img, path_buffer); }
            lv_obj_center(img);
        }
        {
            lv_obj_t *btn = lv_btn_create(panel);
            objects.pesas = btn;
            lv_obj_set_size(btn, 50, 50);
            lv_obj_t *img = lv_img_create(btn);
            ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ICON_GYM.bin");
            if (check_asset_exists(path_buffer)) { lv_img_set_src(img, path_buffer); }
            lv_obj_center(img);
        }
        {
            lv_obj_t *btn = lv_btn_create(panel);
            objects.atacar = btn;
            lv_obj_set_size(btn, 50, 50);
            lv_obj_t *img = lv_img_create(btn);
            ui_helpers_build_asset_path(path_buffer, sizeof(path_buffer), "ICON_ATK.bin");
            if (check_asset_exists(path_buffer)) { lv_img_set_src(img, path_buffer); }
            lv_obj_center(img);
        }
    }
}


// --- IMPLEMENTACIÓN DE LAS FUNCIONES DE ANIMACIÓN Y GESTOS ---

static void animate_panel_in(void) {
    if (objects.actions_panel) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, objects.actions_panel);
        lv_anim_set_values(&a, lv_obj_get_y(objects.actions_panel), 0);
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_start(&a);
    }
}

static void animate_panel_out(void) {
    if (objects.actions_panel) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, objects.actions_panel);
        lv_anim_set_values(&a, lv_obj_get_y(objects.actions_panel), -lv_obj_get_height(objects.actions_panel));
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
        lv_anim_start(&a);
    }
}

static void screen_gesture_event_cb(lv_event_t *e) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_TOP) {
        animate_panel_in();
    }
}

static void panel_gesture_event_cb(lv_event_t *e) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_BOTTOM) {
        animate_panel_out();
    }
}

void delete_screen_main() {
    if (objects.main) {
        lv_obj_del(objects.main);
    }
    ui_helpers_free_background_buffer();
    memset(&objects, 0, sizeof(objects_t));
}

void create_screens() {
    create_screen_main();
}