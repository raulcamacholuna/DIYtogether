/*
Fichero: components/ui/actions/action_performance_mode.c
Último cambio: Desactivado el scroll en la pantalla de modo rendimiento.
Descripción: Se ha añadido la funcionalidad para desactivar el scroll en la pantalla activa al entrar en el modo de rendimiento y se reactiva al salir. Esto se hace mediante las funciones `lv_obj_clear_flag` y `lv_obj_add_flag` con el flag `LV_OBJ_FLAG_SCROLLABLE`.
*/
#include "action_performance_mode.h"
#include "../ui_idle_animation.h"
#include "../helpers.h"
#include "lvgl.h"
#include "esp_log.h"
#include "../assets/images/RSPLS.h"

static const char *TAG = "ACTION_PERF_MODE";
static bool is_performance_mode_active = false;
static lv_obj_t *performance_wallpaper = NULL;
static lv_obj_t *circles_obj[5];
static int8_t selected_circle = -1;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t r;
} circle_t;

static const circle_t circles[] = {
    {82, 93, 25},  // Top
    {136, 141, 25}, // Top-Right
    {114, 218, 25}, // Bottom-Right
    {51, 218, 25},  // Bottom-Left
    {34, 141, 25}   // Top-Left
};

static void circle_event_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);

    // Find which circle was clicked
    for (int i = 0; i < 5; i++) {
        if (obj == circles_obj[i]) {
            selected_circle = i;
            break;
        }
    }

    // Update all circles
    for (int i = 0; i < 5; i++) {
        if (i == selected_circle) {
            lv_obj_set_style_border_color(circles_obj[i], lv_color_hex(0x00FF00), 0);
            lv_obj_set_style_border_width(circles_obj[i], 4, 0);
            lv_obj_set_style_border_opa(circles_obj[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_border_opa(circles_obj[i], LV_OPA_TRANSP, 0);
        }
    }
}

void action_performance_mode_toggle(void) {
    is_performance_mode_active = !is_performance_mode_active;
    ESP_LOGI(TAG, "Cambiando modo rendimiento a: %s", is_performance_mode_active ? "ON" : "OFF");

    if (!g_wallpaper_img) {
        ESP_LOGE(TAG, "El objeto del fondo de pantalla principal (g_wallpaper_img) no está disponible.");
        return;
    }

    if (is_performance_mode_active) {
        // Activar modo rendimiento
        lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
        ui_idle_animation_pause();
        lv_obj_add_flag(g_wallpaper_img, LV_OBJ_FLAG_HIDDEN);

        if (!performance_wallpaper) {
            performance_wallpaper = lv_img_create(lv_screen_active());
            lv_img_set_src(performance_wallpaper, &RSPLS);
            lv_obj_align(performance_wallpaper, LV_ALIGN_CENTER, 0, 0);

            // Create circle objects
            for (int i = 0; i < 5; i++) {
                circles_obj[i] = lv_obj_create(lv_screen_active());
                lv_obj_set_size(circles_obj[i], circles[i].r * 2, circles[i].r * 2);
                lv_obj_set_pos(circles_obj[i], circles[i].x - circles[i].r, circles[i].y - circles[i].r);
                lv_obj_add_flag(circles_obj[i], LV_OBJ_FLAG_CLICKABLE);
                lv_obj_set_style_bg_opa(circles_obj[i], LV_OPA_TRANSP, 0);
                lv_obj_set_style_radius(circles_obj[i], LV_RADIUS_CIRCLE, 0);
                lv_obj_set_style_border_opa(circles_obj[i], LV_OPA_TRANSP, 0);
                lv_obj_add_event_cb(circles_obj[i], circle_event_cb, LV_EVENT_CLICKED, NULL);
            }
        }
        lv_obj_clear_flag(performance_wallpaper, LV_OBJ_FLAG_HIDDEN);
        for (int i = 0; i < 5; i++) {
            if(circles_obj[i]) lv_obj_clear_flag(circles_obj[i], LV_OBJ_FLAG_HIDDEN);
        }

    } else {
        // Desactivar modo rendimiento
        lv_obj_add_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
        if (performance_wallpaper) {
            lv_obj_add_flag(performance_wallpaper, LV_OBJ_FLAG_HIDDEN);
            for (int i = 0; i < 5; i++) {
                if(circles_obj[i]) lv_obj_add_flag(circles_obj[i], LV_OBJ_FLAG_HIDDEN);
            }
        }
        lv_obj_clear_flag(g_wallpaper_img, LV_OBJ_FLAG_HIDDEN);
        ui_idle_animation_resume();
    }
}