/* Fichero: components/ui/core/ui.c */
/* Descripción: Diagnóstico de Causa Raíz: El 'Load access fault' ocurre porque la función delete_screen_main se llamaba a sí misma de forma recursiva. El flujo era: 1.  action_config_mode_start llama a delete_screen_main. 2. delete_screen_main llama a lv_obj_del(g_main_screen_obj). 3. lv_obj_del dispara el evento LV_EVENT_DELETE. 4. Un callback incorrectamente registrado para este evento era la propia función delete_screen_main, provocando una segunda llamada concurrente que intentaba liberar recursos ya en proceso de liberación, causando el crash.
Solución Definitiva: Se ha eliminado la línea que registraba el callback recursivo. La limpieza de la pantalla principal ahora se inicia de forma única y explícita desde el orquestador de acciones, y el callback de limpieza correcto ('main_screen_cleanup_cb' en 'screens.c') se ejecuta una sola vez, garantizando que los recursos se liberen de forma segura. */
/* Último cambio: 20/08/2025 - 07:32 */
#include "ui.h"
#include "screens.h"
#include "ui_action_animations.h"
#include "esp_log.h"

extern lv_obj_t *g_main_screen_obj; 

static const char *TAG = "DIYMON_UI_MAIN";

void ui_preinit(void) {
    ESP_LOGI(TAG, "Pre-inicializando UI: reservando buffer de animación...");
    ui_action_animations_preinit_buffer();
}

void ui_init(void) {
    create_screens();
    
    // [CORRECCIÓN] Se elimina el callback recursivo que causaba el crash.
    // La limpieza ahora se gestiona explícitamente desde el modo de configuración,
    // y el callback correcto ('main_screen_cleanup_cb') ya está registrado en 'screens.c'.
    // if (g_main_screen_obj) {
    //     lv_obj_add_event_cb(g_main_screen_obj, (lv_event_cb_t)delete_screen_main, LV_EVENT_DELETE, NULL);
    // }
    
    lv_screen_load(g_main_screen_obj);
    ESP_LOGI(TAG, "UI modularizada y lista.");
}
