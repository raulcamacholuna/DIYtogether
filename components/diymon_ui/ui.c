/*
 * Archivo: ui.c
 * Descripción: Orquestador principal de la UI de DIYMON.
 * Versión: 7.1 (CORREGIDO para la API de LVGL 9)
 */

#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "ui_priv.h"
#include "diymon_evolution.h"

#include "lvgl.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "DIYMON_UI";

// --- Funciones de gestión de animaciones ---

static void ui_load_diymon_gif(const char* gif_name) {
    if (!g_diymon_gif_obj) {
        ESP_LOGE(TAG, "El objeto GIF (g_diymon_gif_obj) no ha sido creado!");
        return;
    }

    const char* evo_code = diymon_get_current_code();
    static char gif_path[128];

    snprintf(gif_path, sizeof(gif_path), "S:/sdcard/diymon/%s/%s.gif", evo_code, gif_name);
    ESP_LOGI(TAG, "Cargando GIF: %s", gif_path);

    // [CORRECCIÓN] En LVGL 9, para un objeto GIF se usa lv_image_set_src.
    lv_image_set_src(g_diymon_gif_obj, gif_path);
}

void ui_update_diymon_sprite(void) {
    ui_load_diymon_gif("idle");
}

void ui_play_action_animation(const char* action_name) {
    ui_load_diymon_gif(action_name);
}

// --- Funciones de inicialización y control de la UI ---

static void ui_connect_dynamic_actions() {
    ESP_LOGI(TAG, "Conectando acciones al sistema dinámico...");
    if (objects.comer) {
        lv_obj_add_event_cb(objects.comer, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_COMER);
    }
    if (objects.pesas) {
        lv_obj_add_event_cb(objects.pesas, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_EJERCICIO);
    }
    if (objects.atacar) {
        lv_obj_add_event_cb(objects.atacar, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_ATACAR);
    }
}

void ui_init() {
    create_screens();
    ui_connect_dynamic_actions();
    
    // Cargamos la pantalla principal
    lv_screen_load(objects.main);
    
    // Actualizamos el sprite y el fondo DESPUÉS de cargar la pantalla
    ui_update_diymon_sprite();
    ui_update_diymon_background(); // Ahora esta función es reconocida
    
    ESP_LOGI(TAG, "UI de DIYMON inicializada y lista.");
}