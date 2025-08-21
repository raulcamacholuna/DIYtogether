/* Fichero: components/screen_manager/screen_manager.h */
/* Descripción: Se actualiza la cabecera del gestor de pantalla para reflejar la nueva firma de la función de brillo, propagando la capacidad de controlar el guardado en NVS a las capas superiores de la aplicación. */
/* Último cambio: 21/08/2025 - 19:23 */
#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa el gestor de pantalla.
 * 
 * @return ESP_OK si la inicialización es correcta.
 */
esp_err_t screen_manager_init(void);

/**
 * @brief Enciende la pantalla y restaura el último nivel de brillo.
 */
void screen_manager_turn_on(void);

/**
 * @brief Apaga la pantalla y el backlight.
 */
void screen_manager_turn_off(void);

/**
 * @brief Establece el nivel de brillo de la pantalla.
 * 
 * @param percentage Brillo en porcentaje (0-100).
 * @param save_to_nvs Si es true, el valor se guardará en la NVS como preferencia del usuario.
 */
void screen_manager_set_brightness(int percentage, bool save_to_nvs);

/**
 * @brief Devuelve si la pantalla está actualmente apagada.
 * 
 * @return true si la pantalla está apagada, false en caso contrario.
 */
bool screen_manager_is_off(void);


#ifdef __cplusplus
}
#endif

#endif // SCREEN_MANAGER_H
