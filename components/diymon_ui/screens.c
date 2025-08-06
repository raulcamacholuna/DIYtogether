#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "styles.h"
#include "ui_priv.h" // [NUEVO] Para usar la variable global
#include "lvgl.h"


objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

// [NUEVO] Definimos aquí nuestra variable global para el GIF
lv_obj_t *g_diymon_gif_obj = NULL;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 172, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_img_src(obj, "S:/skins/default/fondo.bin", LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        
        // --- [MODIFICADO] CREACIÓN DEL ACTOR PRINCIPAL (UN ÚNICO GIF) ---
        g_diymon_gif_obj = lv_gif_create(parent_obj);
        objects.idle = g_diymon_gif_obj;
        lv_obj_set_pos(g_diymon_gif_obj, -4, 77);
        lv_obj_set_size(g_diymon_gif_obj, 180, 243);
        lv_obj_clear_flag(g_diymon_gif_obj, LV_OBJ_FLAG_SCROLLABLE);
        
        // --- [COMENTADO] Ya no creamos los animimg compilados ---
        /*
        { // Idle
            ...
        }
        { // Comiendo
            ...
        }
        { // ejercicio
            ...
        }
        { // ataque
            ...
        }
        */

        { // SCROLL_UI - Los botones no cambian
            lv_obj_t *obj = lv_obj_create(parent_obj);
            // ... todo tu código para el scroll y los botones (comer, pesas, atacar) se mantiene igual ...
             lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // Comer
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.comer = obj;
                    lv_obj_set_pos(obj, 188, 18);
                    lv_obj_set_size(obj, 65, 59);
                    lv_obj_add_event_cb(obj, action_comer, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_img_src(obj, &img_apple, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // Pesas
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.pesas = obj;
                    lv_obj_set_pos(obj, 271, 18);
                    lv_obj_set_size(obj, 65, 59);
                    lv_obj_add_event_cb(obj, action_ejercicio, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_img_src(obj, &img_pesas, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // Atacar
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.atacar = obj;
                    lv_obj_set_pos(obj, 355, 18);
                    lv_obj_set_size(obj, 60, 60);
                    lv_obj_add_event_cb(obj, action_atacar, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_img_src(obj, &img_ataque, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_main();
}

// ... El resto del fichero (delete_screen_main, create_screens, etc.) se queda igual ...
// ...
// ...
// ...
void delete_screen_main() {
    lv_obj_del(objects.main);
    objects.main = 0;
    objects.idle = 0;
    objects.comiendo = 0;
    objects.ejercicio = 0;
    objects.ataque = 0;
    objects.scroll_ui = 0;
    objects.comer = 0;
    objects.pesas = 0;
    objects.atacar = 0;
}

void tick_screen_main() {
}



typedef void (*create_screen_func_t)();
create_screen_func_t create_screen_funcs[] = {
    create_screen_main,
};
void create_screen(int screen_index) {
    create_screen_funcs[screen_index]();
}
void create_screen_by_id(enum ScreensEnum screenId) {
    create_screen_funcs[screenId - 1]();
}

typedef void (*delete_screen_func_t)();
delete_screen_func_t delete_screen_funcs[] = {
    delete_screen_main,
};
void delete_screen(int screen_index) {
    delete_screen_funcs[screen_index]();
}
void delete_screen_by_id(enum ScreensEnum screenId) {
    delete_screen_funcs[screenId - 1]();
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}