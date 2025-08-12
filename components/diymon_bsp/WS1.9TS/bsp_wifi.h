/*
  Fichero: ./components/diymon_bsp/WS1.9TS/bsp_wifi.h
  Fecha: 12/08/2025 - 05:05 pm
  Último cambio: Actualizadas las declaraciones de funciones para reflejar la implementación.
  Descripción: Interfaz pública del gestor WiFi del BSP. Se han corregido las declaraciones para que coincidan con las funciones realmente exportadas por sp_wifi.c, resolviendo posibles errores de 'implicit declaration'.
*/
#ifndef __BSP_WIFI_H__
#define __BSP_WIFI_H__

#include "esp_err.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa el stack de red (netif y event loop).
 */
void bsp_wifi_init_stack(void);

/**
 * @brief Escanea redes WiFi disponibles y las muestra en el log.
 * @return ESP_OK si el escaneo fue exitoso.
 */
esp_err_t bsp_wifi_scan(void);

/**
 * @brief Inicia el dispositivo en modo Punto de Acceso (AP).
 */
void bsp_wifi_start_ap(void);

/**
 * @brief Inicia el dispositivo en modo Estación (STA) usando credenciales de NVS.
 */
void bsp_wifi_init_sta_from_nvs(void);

/**
 * @brief Espera de forma bloqueante hasta obtener una dirección IP.
 * @param timeout_ms Tiempo máximo de espera en milisegundos.
 * @return true si se obtuvo IP, false en caso de timeout.
 */
bool bsp_wifi_wait_for_ip(uint32_t timeout_ms);

/**
 * @brief Obtiene la dirección IP actual del dispositivo en modo STA.
 * @param ip Puntero a un buffer de caracteres donde se escribirá la IP.
 */
void bsp_wifi_get_ip(char *ip);

#ifdef __cplusplus
}
#endif

#endif // __BSP_WIFI_H__
