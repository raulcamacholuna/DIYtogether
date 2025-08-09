/*
 * Fichero: ./components/diymon_ui/actions.c
 * Fecha: 10/08/2025 - 03:40
 * Último cambio: Adaptada la implementación a la nueva firma de la función.
 * Descripción: Se actualiza la implementación para usar los parámetros directos `action_id` y `idle_obj`, eliminando la necesidad de extraerlos de un objeto de evento.
 */
#include "actions.h"
#include "ui_action_animations.h" 
#include "esp_log.h"

static const char *TAG = "DIYMON_ACTIONS";

// La función ahora recibe los datos directamente, simplificando la lógica.
void execute_diymon_action(diymon_action_id_t action_id, lv_obj_t* idle_obj) {
    ESP_LOGI(TAG, "Evento de acción recibido para ID: %d. Delegando a reproductor.", action_id);
    
    // Se llama al módulo de animaciones visuales con los datos recibidos.
    ui_action_animations_play(action_id, idle_obj);
}