/*
 * =====================================================================================
 *       Filename:  ui.c
 *    Description:  Orquestador principal de la UI de DIYMON.
 *        Version:  5.2 (CORREGIDO para LVGL 8.4.0)
 * =====================================================================================
 */

#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_priv.h"
#include "diymon_evolution.h"

// [CORRECCIÓN] En LVGL 8, solo necesitamos la cabecera principal.
// Las funciones de los widgets se incluyen automáticamente si están activadas en lv_conf.h
#include "lvgl.h"

#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "DIYMON_UI";

static int16_t currentScreen = -1;

static lv_obj_t *getLvglObjectFromIndex(int32_t index) {
    if (index == -1) { return NULL; }
    return ((lv_obj_t **)&objects)[index];
}

// ----- Funciones de gestión de animaciones -----

void ui_update_diymon_sprite(void) {
    if (!g_diymon_gif_obj) {
        ESP_LOGE(TAG, "El objeto GIF (g_diymon_gif_obj) no ha sido creado en screens.c!");
        return;
    }

    const char* evo_code = diymon_get_current_code();
    static char gif_path[64];

    snprintf(gif_path, sizeof(gif_path), "S:/%s/diymon.gif", evo_code);
    ESP_LOGI(TAG, "Cargando sprite principal desde: %s", gif_path);

    lv_gif_set_src(g_diymon_gif_obj, gif_path);
    lv_animimg_set_repeat_count(g_diymon_gif_obj, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(g_diymon_gif_obj);
}

void ui_play_action_animation(const char* action_name) {
    if (!g_diymon_gif_obj) {
        ESP_LOGE(TAG, "El objeto GIF (g_diymon_gif_obj) no ha sido creado!");
        return;
    }

    const char* evo_code = diymon_get_current_code();
    static char gif_path[64];

    snprintf(gif_path, sizeof(gif_path), "S:/%s/acciones/%s.gif", evo_code, action_name);
    ESP_LOGI(TAG, "Lanzando animación de acción desde: %s", gif_path);
    
    lv_gif_set_src(g_diymon_gif_obj, gif_path);
    lv_animimg_set_repeat_count(g_diymon_gif_obj, 1);
    lv_animimg_start(g_diymon_gif_obj);
}

// ----- Funciones de inicialización y control de la UI -----

static void ui_connect_dynamic_actions() {
    ESP_LOGI(TAG, "Conectando acciones al sistema dinámico...");
    if (objects.comer) {
        lv_obj_add_event_cb(objects.comer, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_COMER);
    }
}

void loadScreen(enum ScreensEnum screenId) {
    currentScreen = screenId - 1;
    lv_obj_t *screen = getLvglObjectFromIndex(currentScreen);
    if (screen) {
        lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
    }
}

void ui_init() {
    create_screens();
    ui_connect_dynamic_actions();
    ui_update_diymon_sprite();
    loadScreen(SCREEN_ID_MAIN);
    ESP_LOGI(TAG, "UI de DIYMON inicializada y lista.");
}

void ui_tick() {
    if (currentScreen != -1) {
        tick_screen(currentScreen);
    }
}