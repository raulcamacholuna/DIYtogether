/*
 * Fichero: components/zigbee_comm/include/zigbee_comm.h
 * Último cambio: 24/08/2025 - Adaptado para la API de Zigbee intermedia.
 * Descripción: Interfaz pública para el componente de comunicación Zigbee.
 */
#ifndef ZIGBEE_COMM_H
#define ZIGBEE_COMM_H

#include "esp_zigbee_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// Enum para definir el rol del dispositivo en la red Zigbee
typedef enum {
    ZIGBEE_ROLE_COORDINATOR,
    ZIGBEE_ROLE_ROUTER,
} zigbee_role_t;

// Tipo de dato para los paquetes de minijuegos
typedef struct {
    uint8_t game_id;
    uint8_t payload[32];
    uint8_t len;
} game_packet_t;

// Definición del callback para la recepción de datos
typedef void (*zigbee_data_receive_callback_t)(uint16_t src_addr, game_packet_t* packet);

/**
 * @brief Inicializa el stack de comunicación Zigbee.
 */
esp_err_t zigbee_comm_init(zigbee_role_t role);

/**
 * @brief Inicia el stack Zigbee y la formación/unión a la red.
 */
void zigbee_comm_start(void);

/**
 * @brief Envía un paquete de datos de un minijuego a un dispositivo específico.
 */
esp_err_t zigbee_comm_send_unicast(uint16_t short_addr, const game_packet_t* packet);

/**
 * @brief Registra una función de callback para ser notificado de la llegada de datos.
 */
void zigbee_comm_register_data_callback(zigbee_data_receive_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // ZIGBEE_COMM_H