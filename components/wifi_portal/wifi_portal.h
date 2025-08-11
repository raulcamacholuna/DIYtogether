#ifndef WIFI_PORTAL_H
#define WIFI_PORTAL_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Comprueba si ya existen credenciales WiFi guardadas en la NVS.
 * @return true si se encontraron credenciales, false en caso contrario.
 */
bool wifi_portal_credentials_saved(void);

/**
 * @brief Inicia el portal de configuración WiFi de forma bloqueante.
 * @note Esta función NUNCA retorna. Termina reiniciando el dispositivo.
 */
void wifi_portal_start(void);

/**
 * @brief Borra las credenciales WiFi (SSID y contraseña) de la NVS.
 */
void wifi_portal_erase_credentials(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_PORTAL_H