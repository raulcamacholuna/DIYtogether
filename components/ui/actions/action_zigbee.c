/* Fichero: components/ui/actions/action_zigbee.c */
/* Último cambio: Implementación de funciones de acción Zigbee para la UI. */
/* Descripción: Este archivo contiene las implementaciones de las funciones de acción Zigbee
 * invocadas desde la interfaz de usuario. Incluye la lógica para iniciar un juego
 * como coordinador y unirse a un juego como router, utilizando la API de zigbee_comm.
 */

#include "action_zigbee.h"
#include "zigbee_comm.h"
#include "esp_log.h"

static const char *TAG = "UI_ACTION_ZIGBEE";

void action_zigbee_create_game()
{
    ESP_LOGI(TAG, "Attempting to create Zigbee game (Coordinator role)");
    // Lógica para crear el juego Zigbee (Coordinador)
    // Esto debería inicializar el stack Zigbee como coordinador y formar una red
    esp_err_t err = zigbee_comm_init(ZIGBEE_ROLE_COORDINATOR);
    if (err == ESP_OK) {
        zigbee_comm_start();
        ESP_LOGI(TAG, "Zigbee game creation initiated.");
    } else {
        ESP_LOGE(TAG, "Failed to initialize Zigbee for game creation: %s", esp_err_to_name(err));
    }
}

void action_zigbee_join_game()
{
    ESP_LOGI(TAG, "Attempting to join Zigbee game (Router role)");
    // Lógica para unirse al juego Zigbee (Router)
    // Esto debería inicializar el stack Zigbee como router y buscar una red existente
    esp_err_t err = zigbee_comm_init(ZIGBEE_ROLE_ROUTER);
    if (err == ESP_OK) {
        zigbee_comm_start();
        ESP_LOGI(TAG, "Zigbee game joining initiated.");
    } else {
        ESP_LOGE(TAG, "Failed to initialize Zigbee for game joining: %s", esp_err_to_name(err));
    }
}
