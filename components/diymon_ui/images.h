#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_monstruo;
extern const lv_img_dsc_t img_fondo;
extern const lv_img_dsc_t img_2_resized;
extern const lv_img_dsc_t img_3_resized;
extern const lv_img_dsc_t img_1_resized;
extern const lv_img_dsc_t img_c1;
extern const lv_img_dsc_t img_c2;
extern const lv_img_dsc_t img_c3;
extern const lv_img_dsc_t img_c4;
extern const lv_img_dsc_t img_apple;
extern const lv_img_dsc_t img_pesas;
extern const lv_img_dsc_t img_p1;
extern const lv_img_dsc_t img_p2;
extern const lv_img_dsc_t img_p3;
extern const lv_img_dsc_t img_ah1;
extern const lv_img_dsc_t img_ah2;
extern const lv_img_dsc_t img_ah3;
extern const lv_img_dsc_t img_ah4;
extern const lv_img_dsc_t img_ataque;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[19];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/