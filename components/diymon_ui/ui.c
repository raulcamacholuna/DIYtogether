/*
 * =====================================================================================
 *       Filename:  ui.c
 *    Description:  Orquestador principal de la UI de DIYMON.
 *                  Gestiona las animaciones desde la SD.
 *        Version:  5.1 (Limpio y corregido para LVGL 8)
 * =====================================================================================
 */

#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_priv.h"
#include "diymon_evolution.h"

// [IMPORTANTE] Cabeceras necesarias para los widgets que usamos
#include "lvgl.h"
#include "src/widgets/lv_gif.h"
#include "src/widgets/lv_animimg.h" // Para controlar los GIFs

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

/**
 * @brief Actualiza el sprite principal del DIYMON basándose en su código evolutivo actual.
 *        Esta animación (idle) se repite infinitamente.
 */
void ui_update_diymon_sprite(void) {
    if (!g_diymon_gif_obj) {
        ESP_LOGE(TAG, "El objeto GIF del DIYMON (g_diymon_gif_obj) no ha sido creado en screens.c!");
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

/**
 * @brief Lanza una animación de acción que se reproduce una sola vez.
 */
void ui_play_action_animation(const char* action_name) {
    if (!g_diymon_gif_obj) {
        ESP_LOGE(TAG, "El objeto GIF del DIYMON (g_diymon_gif_obj) no ha sido creado!");
        return;
    }

    const char* evo_code = diymon_get_current_code();
    static char gif_path[64];

    snprintf(gif_path, sizeof(gif_path), "S:/%s/acciones/%s.gif", evo_code, action_name);
    ESP_LOGI(TAG, "Lanzando animación de acción desde: %s", gif_path);
    
    lv_gif_set_src(g_diymon_gif_obj, gif_path);
    
    // Le decimos que se reproduzca una sola vez
    lv_animimg_set_repeat_count(g_diymon_gif_obj, 1);
    
    // Y la reiniciamos
    lv_animimg_start(g_diymon_gif_obj);

    // NOTA: Para que vuelva a la animación 'idle' automáticamente,
    // necesitaremos un temporizador o un evento 'LV_EVENT_READY' en el futuro.
}


// ----- Funciones de inicialización y control de la UI -----

static void ui_connect_dynamic_actions() {
    ESP_LOGI(TAG, "Conectando acciones al sistema dinámico...");
    if (objects.comer) {
        // La función 'action_comer' ya no es necesaria, conectamos directamente al orquestador.
        lv_obj_add_event_cb(objects.comer, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_COMER);
    }
    // ... aquí conectarías otros botones como 'pesas' o 'atacar' de la misma forma ...
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
    
    // Al iniciar la UI, cargamos el sprite correspondiente al estado guardado en memoria.
    ui_update_diymon_sprite();

    loadScreen(SCREEN_ID_MAIN);
    ESP_LOGI(TAG, "UI de DIYMON inicializada y lista.");
}

void ui_tick() {
    if (currentScreen != -1) {
        tick_screen(currentScreen);
    }
}