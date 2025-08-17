/* Fecha: 17/08/2025 - 01:31 
# Fichero: components/ui/ui.c
# Último cambio: Eliminada la función 'ui_connect_actions' y el callback de eventos centralizado para completar la refactorización de botones.
# Descripción: Orquestador principal de la UI. Se ha eliminado la lógica de conexión de eventos, ya que ahora cada módulo de botón es responsable de conectar su propio callback. Esta es la limpieza final de la refactorización, haciendo que este fichero sea agnóstico a los botones específicos.
*/
#include "ui.h"
#include "screens.h"
#include "ui_action_animations.h"
#include "esp_log.h"

extern lv_obj_t *g_main_screen_obj; 

static const char *TAG = "DIYMON_UI_MAIN";

// La función 'ui_connect_actions' y su 'button_event_cb' han sido eliminados.
// La conexión de eventos ahora está encapsulada dentro de cada módulo de botón.

void ui_preinit(void) {
    ESP_LOGI(TAG, "Pre-inicializando UI: reservando buffer de animación...");
    ui_action_animations_preinit_buffer();
}

void ui_init(void) {
    create_screens();
    
    if (g_main_screen_obj) {
        // La llamada a ui_connect_actions() ha sido eliminada.
        lv_obj_add_event_cb(g_main_screen_obj, (lv_event_cb_t)delete_screen_main, LV_EVENT_DELETE, NULL);
    }
    
    lv_screen_load(g_main_screen_obj);
    ESP_LOGI(TAG, "UI modularizada y lista.");
}
