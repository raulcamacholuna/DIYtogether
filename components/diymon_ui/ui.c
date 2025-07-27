/*
 * =====================================================================================
 *       Filename:  ui.c
 *    Description:  Punto de entrada de la UI y conector del sistema de acciones dinámicas.
 *        Version:  4.2 (Final y Correcto)
 * =====================================================================================
 */
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "DIYMON_UI";

#if defined(EEZ_FOR_LVGL)
// Rama de compatibilidad con EEZ Flow (no usada)
#else
// Rama principal de nuestro proyecto
static int16_t currentScreen = -1;

static lv_obj_t *getLvglObjectFromIndex(int32_t index) {
    if (index == -1) { return NULL; }
    return ((lv_obj_t **)&objects)[index];
}

static void ui_connect_dynamic_actions() {
    ESP_LOGI(TAG, "Conectando acciones al sistema dinámico...");
    if (objects.comer) {
        lv_obj_remove_event_cb(objects.comer, action_comer);
        lv_obj_add_event_cb(objects.comer, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_COMER);
    }
    if (objects.pesas) {
        lv_obj_remove_event_cb(objects.pesas, action_ejercicio);
        lv_obj_add_event_cb(objects.pesas, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_EJERCICIO);
    }
    if (objects.atacar) {
        lv_obj_remove_event_cb(objects.atacar, action_atacar);
        lv_obj_add_event_cb(objects.atacar, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_ATACAR);
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
    loadScreen(SCREEN_ID_MAIN);
    ESP_LOGI(TAG, "UI de DIYMON inicializada y lista.");
}

void ui_tick() {
    if (currentScreen != -1) {
        tick_screen(currentScreen);
    }
}
#endif