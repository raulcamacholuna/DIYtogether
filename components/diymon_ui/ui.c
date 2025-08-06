/*
 * =====================================================================================
 *       Filename:  ui.c
 *    Description:  Punto de entrada de la UI y conector del sistema de acciones dinámicas.
 *                  Ahora gestiona las animaciones del DIYMON.
 *        Version:  5.0 (Con Animaciones desde SD)
 * =====================================================================================
 */
#include "ui.h"
#include "screens.h"  // Para create_screens()
#include "actions.h"    // Para execute_diymon_action()
#include "ui_priv.h"    // [NUEVO] Para compartir el objeto GIF
#include "diymon_evolution.h" // [NUEVO] Para saber el estado del DIYMON

#include "esp_log.h"
#include <stdio.h>    // [NUEVO] Para usar snprintf
#include <string.h>

static const char *TAG = "DIYMON_UI";

#if defined(EEZ_FOR_LVGL)
// Rama de compatibilidad con EEZ Flow (no usada)
#else

static int16_t currentScreen = -1;

// Objeto GIF global que representa a nuestro DIYMON en la pantalla
// Se inicializa en screens.c y se usa aquí para cambiar las animaciones.
// lv_obj_t *g_diymon_gif_obj = NULL; // Movido a screens.c

static lv_obj_t *getLvglObjectFromIndex(int32_t index) {
    if (index == -1) { return NULL; }
    return ((lv_obj_t **)&objects)[index];
}

// ----- [NUEVO] Funciones para gestionar las animaciones -----

/**
 * @brief Actualiza el sprite principal del DIYMON basándose en su código evolutivo actual.
 */
void ui_update_diymon_sprite(void) {
    if (!g_diymon_gif_obj) {
        ESP_LOGE(TAG, "El objeto GIF del DIYMON no ha sido creado!");
        return;
    }

    const char* evo_code = diymon_get_current_code();
    static char gif_path[64]; // 'static' para que no viva en el stack de una sola llamada

    // Construimos la ruta al GIF principal de la evolución actual
    snprintf(gif_path, sizeof(gif_path), "S:/%s/diymon.gif", evo_code);

    ESP_LOGI(TAG, "Cargando sprite principal desde: %s", gif_path);
    lv_gif_set_src(g_diymon_gif_obj, gif_path);
    lv_gif_restart(g_diymon_gif_obj);
}

/**
 * @brief Lanza una animación de acción (como comer, atacar, etc.).
 * @param action_name El nombre del fichero GIF de la acción (ej: "comer").
 */
void ui_play_action_animation(const char* action_name) {
    if (!g_diymon_gif_obj) {
        ESP_LOGE(TAG, "El objeto GIF del DIYMON no ha sido creado!");
        return;
    }

    const char* evo_code = diymon_get_current_code();
    static char gif_path[64];

    // Construimos la ruta a la animación de la acción
    snprintf(gif_path, sizeof(gif_path), "S:/%s/acciones/%s.gif", evo_code, action_name);
    
    ESP_LOGI(TAG, "Lanzando animación de acción desde: %s", gif_path);
    
    // Cambiamos la fuente al GIF de la acción
    lv_gif_set_src(g_diymon_gif_obj, gif_path);
    
    // Le decimos a LVGL que NO repita la animación. Cuando termine, se quedará en el último frame.
    lv_gif_set_loop(g_diymon_gif_obj, LV_GIF_LOOP_OFF);
    lv_gif_restart(g_diymon_gif_obj);

    // FUTURO: Podríamos usar un temporizador para, cuando termine la animación,
    // volver a poner la animación principal (idle).
    // Por ahora, para volver a la normalidad, tendrás que pulsar otro botón.
}


// ----- Funciones de inicialización y conexión -----

static void ui_connect_dynamic_actions() {
    ESP_LOGI(TAG, "Conectando acciones al sistema dinámico...");
    if (objects.comer) {
        lv_obj_remove_event_cb(objects.comer, action_comer);
        lv_obj_add_event_cb(objects.comer, execute_diymon_action, LV_EVENT_CLICKED, (void *)ACTION_ID_COMER);
    }
    // ... tus otras conexiones de botones ...
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
    
    // [MODIFICADO] Al iniciar la UI, cargamos el sprite correspondiente al estado guardado.
    ui_update_diymon_sprite();

    loadScreen(SCREEN_ID_MAIN);
    ESP_LOGI(TAG, "UI de DIYMON inicializada y lista.");
}

void ui_tick() {
    if (currentScreen != -1) {
        tick_screen(currentScreen);
    }
}
#endif