/*
 * Archivo: ui.c
 * Versión: 9.1 (CORREGIDO - Include que faltaba)
 */
#include "ui.h"
#include "screens.h"
#include "ui_priv.h" // <-- ¡EL INCLUDE QUE FALTABA Y CAUSABA EL ERROR!
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "DIYMON_UI";

// Esta función se puede eliminar, pero la dejamos como un stub por si alguna
// parte del código todavía la llama.
void ui_update_diymon_sprite(void) {
    // No hace nada
}

void ui_init() {
    create_screens();
    // Ahora 'objects' es conocido gracias a "ui_priv.h"
    lv_screen_load(objects.main); 
    ui_update_diymon_background();
    ESP_LOGI(TAG, "UI (Carga Manual) inicializada y lista.");
}