#ifndef WIFI_PORTAL_H
#define WIFI_PORTAL_H

#include "esp_err.h"
#include <stdbool.h> 

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define los posibles resultados de la ejecución del portal.
 */
typedef enum {
    PORTAL_CONFIG_SAVED,  // El usuario guardó las credenciales.
    PORTAL_CONFIG_SKIPPED // El usuario presionó el botón de omitir.
} wifi_portal_result_t;

/**
 * @brief Comprueba si ya existen credenciales WiFi guardadas en la NVS.
 * 
 * @return true si se encontraron credenciales, false en caso contrario.
 */
bool wifi_portal_credentials_saved(void);

/**
 * @brief Inicia el portal de configuración WiFi de forma bloqueante.
 * 
 * @return El resultado de la interacción del usuario (guardado u omitido).
 */
wifi_portal_result_t wifi_portal_start(void);


#ifdef __cplusplus
}
#endif

#endif // WIFI_PORTAL_H