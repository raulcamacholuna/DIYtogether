#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 172, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_img_src(obj, &img_fondo, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // Idle
            lv_obj_t *obj = lv_animimg_create(parent_obj);
            objects.idle = obj;
            lv_obj_set_pos(obj, -4, 77);
            lv_obj_set_size(obj, 180, 243);
            static const lv_img_dsc_t *images[3] = {
                &img_1_resized,
                &img_2_resized,
                &img_3_resized,
            };
            lv_animimg_set_src(obj, (const void **)images, 3);
            lv_animimg_set_duration(obj, 3000);
            lv_animimg_set_repeat_count(obj, LV_ANIM_REPEAT_INFINITE);
            lv_animimg_start(obj);
            lv_obj_add_event_cb(obj, action_idle, LV_EVENT_READY, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // Comiendo
            lv_obj_t *obj = lv_animimg_create(parent_obj);
            objects.comiendo = obj;
            lv_obj_set_pos(obj, -4, 77);
            lv_obj_set_size(obj, 180, 243);
            static const lv_img_dsc_t *images[4] = {
                &img_c1,
                &img_c2,
                &img_c3,
                &img_c4,
            };
            lv_animimg_set_src(obj, (const void **)images, 4);
            lv_animimg_set_duration(obj, 2000);
            lv_animimg_set_repeat_count(obj, 1);
            lv_animimg_start(obj);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        }
        {
            // ejercicio
            lv_obj_t *obj = lv_animimg_create(parent_obj);
            objects.ejercicio = obj;
            lv_obj_set_pos(obj, -4, 77);
            lv_obj_set_size(obj, 180, 243);
            static const lv_img_dsc_t *images[3] = {
                &img_p1,
                &img_p2,
                &img_p3,
            };
            lv_animimg_set_src(obj, (const void **)images, 3);
            lv_animimg_set_duration(obj, 2000);
            lv_animimg_set_repeat_count(obj, 2);
            lv_animimg_start(obj);
            lv_obj_add_event_cb(obj, action_idle, LV_EVENT_READY, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // ataque
            lv_obj_t *obj = lv_animimg_create(parent_obj);
            objects.ataque = obj;
            lv_obj_set_pos(obj, -8, 77);
            lv_obj_set_size(obj, 180, 243);
            static const lv_img_dsc_t *images[4] = {
                &img_ah2,
                &img_ah3,
                &img_ah1,
                &img_ah4,
            };
            lv_animimg_set_src(obj, (const void **)images, 4);
            lv_animimg_set_duration(obj, 3000);
            lv_animimg_set_repeat_count(obj, 1);
            lv_animimg_start(obj);
            lv_obj_add_event_cb(obj, action_idle, LV_EVENT_READY, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // SCROLL_UI
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.scroll_ui = obj;
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
